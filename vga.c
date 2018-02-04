#include <vga.h>
#include <string.h>
#include <console.h>
#include <hal.h>
#include <font_loader.h>
#include <edge_list.h>

uint8_t *_videoMem = (uint8_t *)0xA0000;

// These allow for different resolutions to be used
int _height = 200;
int _width = 320;
uint8_t _chainMode = 1;

int _currentX = 0;
int _currentY = 0;

int _lineStyle = 0;

// Sets the resolution variables
void SetResolution(int width, int height, uint8_t chainMode)
{
    _width = width;
    _height = height;
    _chainMode = chainMode;
}


// Sets the given values of RGB at the given index by outputting to the relevant ports using the hardware access layer
void SetColourPalette(uint8_t red, uint8_t green, uint8_t blue, int index)
{
    HAL_OutputByteToPort(0x3C8, index);
    HAL_OutputByteToPort(0x3C9, red);
    HAL_OutputByteToPort(0x3C9, green);
    HAL_OutputByteToPort(0x3C9, blue);
}

// Sets the value to use for modulo
void SetLineStyle(int dashLength)
{
    // 1 is added to get the full pattern length (dash + empty pixel)
    _lineStyle = dashLength + 1; 
}

void ClearScreen()
{
    // Sets the entire video memory block to 0
    memset(_videoMem, 0, _width * _height); 
}

// Returns the value at the specified pixel
uint8_t* GetPixel(int x, int y)
{
    if (x < _width && y < _height)
    {
        if(_chainMode)
        {
            return _videoMem + _width * y + x;
        } else {
            int plane = x&3;
            HAL_OutputByteToPort(0x3C4, 0x02);
            HAL_OutputByteToPort(0x3C5, 1 << plane);
            return (_videoMem + ((_width * y + x) / 4));
        }
    }  
}

// Set the pixel at x, y to the specified colour
// If chainMode is 1 then use the standard linear access
// Without chain mode we have to determine the plane to write to, select it and then work out the offset
void SetPixel(unsigned int x, unsigned int y, unsigned int colour)
{
    if (x < _width && y < _height)
    {
        if(_chainMode)
        {
            *(_videoMem + _width * y + x) = colour;
        } else {
            // we only want the last 3 bits of x to determine the plane
            int plane = x & 3;
            // Write to the sequence controller to select the map mask register
            HAL_OutputByteToPort(0x3C4, 0x02);
            // Write the plane to the data port
            HAL_OutputByteToPort(0x3C5, 1 << plane);
            // Determine the offset and write the colour to the relevant byte
            *(_videoMem + ((_width * y + x) / 4)) = colour;
        }
    }
}

// Set the values of the stored _currentX and _currentY
// This is used to set the start point for most draw calls
bool MoveTo(int x, int y)
{
    // check that x and y are within the bounds of the canvas
    if (x >= _width || y >= _height)
    {
        return false;
    }
    _currentX = x;
    _currentY = y;

    return true;
}

// Draws a character to the canvas using the font arrary in font_loader.h
void DrawChar(char c, uint8_t colour) 
{
    uint8_t i,j;

    // Convert the character to an index
    c = c & 0x7F; // AND the char with a mask of 01111111
    if (c < ' ') { 
        c = 0; // If c is less than lowest possible value, space/' ' then set to 0
    } else {
        c -= ' '; // Else minus the value of space/' ' as the array starts at space/' '
    }
    // Get the char from the bitmap array
    const uint8_t* chr = font[c];
    // Draw pixels
    for (int i = 0; i < CHAR_WIDTH; i++) {
        for (int j = 0; j < CHAR_HEIGHT; j++) {
            if (chr[i] & (1 << j)) { // check each bit of the byte by masking with a single 1
                SetPixel(_currentX + i, _currentY + j, colour);
            }
        }
    }
}

// Draws a string to the canvas using the font arrary in font_loader.h
void DrawString(const char* str, uint8_t colour)
{
    // while string is not finished draw the next char
    while (*str) {
        DrawChar(*str++, colour);
        // increase current x by the width of a char
        _currentX += CHAR_WIDTH;
    }
}

// Returns the sign of x
int Sign(int x)
{
    return x > 0 ? 1: (x < 0 ? -1: 0);
}

// Returns the absolute value of x
int Abs(int x)
{
    return x < 0 ? -x : x;
}

// Draws a line from the _currentX and _currentY to the given endX and endY
bool LineTo(int endX, int endY, uint8_t colour)
{
    int dx, dy, error, s1, s2, temp;
    int swap = 0;

    dx = Abs(endX - _currentX);
    dy = Abs(endY - _currentY);
    s1 = Sign(endX - _currentX);
    s2 = Sign(endY - _currentY);

    // Check if dx or dy is larger
    // If dy is larger than dx, then swap dx and dy so that we can work with lines outside of 0 <= gradient <= 1
    if(dy > dx)
    {
        temp = dx; 
        dx = dy; 
        dy = temp; 
        swap = 1;
    }


    // Calculate the first value of d
    error = 2 * dy - dx;
    
    int i;
    for(i = 0; i < dx; i++)
    {
        // If line style is 1 or less then it is set wrongly or turned off.
        if (_lineStyle <= 1) {
            SetPixel(_currentX, _currentY, colour);
        }
        else
        {
            // If a valid line style is set then only print when the result is not 0 (loops the line pattern)
            if (i % _lineStyle != 0)
            {
                SetPixel(_currentX, _currentY, colour);
            }
        }
        
        // if the error is greater than or equal to zero then we need to increment the secondary dimension
        while (error >= 0)
        { 
            error = error - 2 * dx; 
            if (swap) 
            {
                // We increment by sign rather than 1 so that we can account of lines with negative gradients
                _currentX += s1; 
            }
            else 
            {
                _currentY += s2; 
            }
        }
        
        error = error + 2 * dy; 
        // increment the dimension with a larger delta as determined by swap
        if (swap) 
        {
            _currentY += s2; 
        }
        else 
        {
            _currentX += s1;
        }
    }
}

// The starting point of the rectangle is set by a call to MoveTo in User_DrawRectangle
// As such we assume _currentX and _currentY are at the top left of the rectangle
void DrawRectangle(int width, int height, uint8_t colour)
{
    // draw each edge of the rectangle using LineTo
    int endX = _currentX + width;
    LineTo(endX, _currentY, colour);

    int endY = _currentY + height;
    LineTo(_currentX, endY, colour);

    endX = _currentX - width;
    LineTo(endX, _currentY, colour);

    endY = _currentY - height;
    LineTo(_currentX, endY, colour);
}

// The starting point of the rectangle is set by a call to MoveTo in User_DrawRectangle
// As such we assume _currentX and _currentY are at the top left of the rectangle
void FillRectangle(int width, int height, uint8_t colour)
{
    // record the previous line style and set to zero so it doesn't interfere with our filling
    int previousLineStyle = _lineStyle;
    SetLineStyle(0);
    int tmp = _currentX;
    for(int i = 0; i < height; i++)
    {
        // draw a line for each edge of the rectangle
        LineTo(_currentX + width, _currentY, colour);
        MoveTo(tmp, _currentY + 1);
    }
    // change linestyle back to the original value so as not to interfere with the users plans
    SetLineStyle(previousLineStyle);
}

// Draw a circle with the centre point being _currentX and _currentY
void DrawCircle(int radius, uint8_t colour)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;

    // set the inital error value
    int error = dx - (radius << 1);

    while (x >= y)
    {
        // Draw the pixel and all it's mirrors for the first iteration
        SetPixel(_currentX + x, _currentY + y, colour);
        SetPixel(_currentX + y, _currentY + x, colour);
        SetPixel(_currentX - y, _currentY + x, colour);
        SetPixel(_currentX - x, _currentY + y, colour);
        SetPixel(_currentX - x, _currentY - y, colour);
        SetPixel(_currentX - y, _currentY - x, colour);
        SetPixel(_currentX + y, _currentY - x, colour);
        SetPixel(_currentX + x, _currentY - y, colour);

        // Check the value of error and make adjustments accordingly
        if (error <= 0)
        {
            y++;
            // update the error value
            error += dy;
            dy += 2;
        }
        if (error > 0)
        {
            x--;
            dx += 2;
            // update the error value
            error += (-radius << 1) + dx;
        }
    }
}

// Works the same as DrawCircle however draws lines between matching y pairs to fill in the centre.
void FillCircle(int radius, uint8_t colour)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int error = dx - (radius << 1);

    int centreX = _currentX;
    int centreY = _currentY;

    // record the previous line style and set to zero so it doesn't interfere with our filling
    int previousLineStyle = _lineStyle;
    SetLineStyle(0);
    while (x >= y)
    {
        // Draw lines between horizontal mirror pairs
        MoveTo(centreX - x, centreY + y);
        LineTo(centreX + x, centreY + y, colour);

        MoveTo(centreX - x, centreY - y);
        LineTo(centreX + x, centreY - y, colour);

        MoveTo(centreX - y, centreY + x);
        LineTo(centreX + y, centreY + x, colour);

        MoveTo(centreX - y, centreY - x);
        LineTo(centreX + y, centreY - x, colour);

        // as in DrawCircle, increment the appropriate values based on error, ready for next iteration
        if (error <= 0)
        {
            y++;
            error += dy;
            dy += 2;
        }
        if (error > 0)
        {
            x--;
            dx += 2;
            error += (-radius << 1) + dx;
        }
    }
    // change linestyle back to the original value so as not to interfere with the users plans
    SetLineStyle(previousLineStyle);
}

// Draws lines for each edge of the polygon, very similar to DrawRectangle
void DrawPolygon(int *coordinates, int numberOfVertices, uint8_t colour)
{ 
    // counter to help us move through the coordinates array
    int counter = 0;
    MoveTo(*(coordinates + counter), *(coordinates + counter + 1));
    counter += 2;
    for (int i = 0; i < numberOfVertices - 1; i++)
    {
        LineTo(*(coordinates + counter), *(coordinates + counter + 1), colour);
        counter += 2;
    }
    // Draw the final line back to the start point
    LineTo(*coordinates, *(coordinates + 1), colour);
}

// Floodfill from the given point anything within the outline of the colour given
void FloodFill(int startX, int startY, uint8_t colour)
{
    // if the colour to set is black then shape will not be seen anyway
    if (colour == 0)
    {
        return;
    }
    // if the pixel is already the required colour then we may have reached an edge or an already filled section
    if (*GetPixel(startX, startY) == colour)
    {
        return;
    }

    // If we pass the above two checks then set the pixel
    SetPixel(startX, startY, colour);

    // spawn recursive calls to check the north, east, south and west pixel from the current pixel
    FloodFill(startX, startY + 1, colour);
    FloodFill(startX, startY - 1, colour);
    FloodFill(startX + 1, startY, colour);
    FloodFill(startX - 1, startY, colour);
}

// Storage for the edge buckets and list nodes we will need for filling polygon
// I had issues with these being local variables while developing scanline and so I made them global.
// Having spent 15+ hours writing scanline I'm too scared to try and improve this unless I have excess time towards the end.
edge _edges[10];
node _nodes[20];
int _edgeIndex = 0;
int _nodeIndex = 0;

// method to get ref to the next empty edge object
edge * mallocEdge()
{
    if (_edgeIndex < 10)
    {
        return &(_edges[_edgeIndex++]);
    }
    return NULL;
}
// method to get ref to the next empty node object
node * mallocNode()
{
    if (_nodeIndex < 20)
    {
        return &(_nodes[_nodeIndex++]);
    }
    return NULL;
}

// sets all the structs back to null ready for the next polygon fill
// reset the indexs that are used by mallocEdge and mallocNode
void ResetPolygonMemory()
{
    for (int i = 0; i < 20; i++)
    {
        _nodes[i].edge = NULL;
        _nodes[i].next = NULL;
    }
    _nodeIndex = 0;
    _edgeIndex = 0;
}

// creates and returns an edge bucket given two vertices
edge * CreateEdge(int x0, int y0, int x1, int y1)
{
    // check that the first point has a lower x, if not swap them
    if (x0 > x1)
    {
        int tmpX = x0;
        int tmpY = y0;

        x0 = x1;
        y0 = y1;

        x1 = tmpX;
        y1 = tmpY;
    }

    // set the edge struct
    edge *edge = mallocEdge();
    edge->dX = Abs(x0 - x1);
    edge->dY = Abs(y0 - y1);
    edge->sum = 0;
    
    // check which direction the line goes based on the min and max y and setup accordingly
    if (y0 < y1)
    {
        edge->yMax = y1;
        edge->yMin = y0;
        edge->x = x0;
        edge->sign = 1;
    }
    else 
    {
        edge->yMax = y0;
        edge->yMin = y1;
        edge->x = x1;
        edge->sign = -1;
    }
    return edge;
}

// put together the edge table given an array of coordinates
node * CreateEdgeTable(int *coordinates, int numberOfVertices, node **edgeTableHead)
{
    // Process each edge of the shape and create buckets for them and populate to the edgeTable
    int counter = 0;
    node *newNode;
    for (int i = 0; i < numberOfVertices - 1; i++)
    {
        // Do not process horizontal lines
        if (coordinates[counter+1] != coordinates[counter+3])
        {
            // Allocate a node
            newNode = mallocNode();
            // Allocate an edge and set the node edge
            newNode->edge = CreateEdge(coordinates[counter], coordinates[counter+1], coordinates[counter+2], coordinates[counter+3]);
            // Node not in list yet so set next to NULL
            newNode->next = NULL;
            AddToList(edgeTableHead, newNode);
            
        }
        // increment the counter ready for the next vertex
        counter += 2;
    }
    // Process the final edge from the last vertex to the first
    // again, do not process horizontal lines
    if (coordinates[counter+1] != coordinates[1])
    {
        newNode = mallocNode();
        newNode->edge = CreateEdge(coordinates[counter], coordinates[counter+1], coordinates[0], coordinates[1]);
        newNode->next = NULL;
        AddToList(edgeTableHead, newNode);
    }
    return *edgeTableHead;
}

// draws a filled polygon given an array of ordered coordinates, using scanline
void FillPolygon(int *coordinates, int numberOfVertices, uint8_t colour)
{
    // Declare the lists we will need
    node *edgeTableHead = NULL;
    node *activeListHead = NULL;

    // Create edgeTable List
    CreateEdgeTable(coordinates, numberOfVertices, &edgeTableHead);
    // Sort the edgeTable list by yMin
    SortEdgeListByY(edgeTableHead);

    // set scanline to lowest y now that table is sorted
    int scanline = edgeTableHead->edge->yMin;

    node *current;
    int edgeTableSize, activeListSize;

    // Record previous line style and set to zero so it does not interfere
    int previousLineStyle = _lineStyle;
    SetLineStyle(0);

    // While the edge table is populated keep processing the edges
    while(edgeTableHead != NULL)
    {
        // Check AL for any edges to be removed (and remove from ET also)    
        current = activeListHead;    
        while (current != NULL)
        {
            if (current->edge->yMax == scanline)
            {
                RemoveFromList(&activeListHead, current->edge);
                RemoveFromList(&edgeTableHead, current->edge);
            }
            current = current->next;
        }
        // Check the edgeTable for any lines which should be added to the Active List
        current = edgeTableHead;
        while (current != NULL)
        {
            if (current->edge->yMin == scanline)
            {
                node *node = mallocNode();
                node->edge = current->edge;
                node->next = NULL;
                AddToList(&activeListHead, node);
            }
            current = current->next;
        }
        //Sort AL by X
        SortEdgeListByX(activeListHead); 
        //Fill in scanline between pairs (move from x to x, flipping the parity of the printpixel bool)
        bool parity = false;
        node *current = activeListHead;
        while (current != NULL)
        {
            MoveTo(current->edge->x, scanline);
            current = current->next;
            LineTo(current->edge->x, scanline, colour);
            current = current->next;
        }
        //increment scanline
        scanline++;
        //increment all x's in the AL based on their slope (+/-1)
        current = activeListHead;
        while(current != NULL)
        {
            if (current->edge->dX != 0)
            {
                current->edge->sum += current->edge->dX;
            }
            while (current->edge->sum >= current->edge->dY)
            {
                current->edge->x += current->edge->sign;
                current->edge->sum -= current->edge->dY;
            }
            current = current->next;
        }
    }
    // Set the line style back to the previous value
    SetLineStyle(previousLineStyle);
    // clear all the array entries to NULL
    ResetPolygonMemory();
}