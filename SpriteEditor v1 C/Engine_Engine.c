void DrawPixel(U32 x, U32 y, I32 color);
void FillScreen(I32 color);
void FillRectangle(U32 x, U32 y, U32 width, U32 height, I32 color);
void DrawRectangle(U32 x, U32 y, U32 width, U32 height, U32 wide, I32 color);
void DrawLine(U32 x0, U32 y0, U32 x1, U32 y1, I32 color);



void DrawPixel(U32 x, U32 y, I32 color){
    I32 *pixel = back_buffer.memory;
    
    if(bar_placement == 1){
        pixel += x + back_buffer.width * (y + black_bar);
    }
    if(bar_placement == 2){
        pixel += black_bar + x + back_buffer.width * y;
    }
    *pixel = color;
}

void FillScreen(I32 color){
    for(U32 i = 0; i < draw_surface.height; i++){
        for(U32 j = 0; j < draw_surface.width; j++){
            DrawPixel(j, i, color);
        }
    }
    
}

void FillRectangle(U32 x, U32 y, U32 width, U32 height, I32 color){
    for(U32 i = y*resolution; i < (y + height)*resolution; i++){
        for(U32 j = x*resolution; j < (x + width)*resolution; j++){
            DrawPixel(j, i, color);
        }
    }
    
}

void DrawRectangle(U32 x, U32 y, U32 width, U32 height, U32 wide, I32 color){
    FillRectangle(x, y, width, wide, color);
    FillRectangle(x, y, wide, height, color);
    FillRectangle(x + width - wide, y, wide, height, color);
    FillRectangle(x, y + height - wide, width, wide, color);
}

//doesnt work on with res
void DrawLine(U32 x0, U32 y0, U32 x1, U32 y1, I32 color){
    
    I32 dx, dy, de;
    U32 x, y;
    U32 c;
    
    if(x0 > x1 && y0 > y1){
        
        I32 tx, ty;
        
        tx = x1;
        ty = y1;
        
        x1 = x0; 
        y1 = y0;
        
        x0 = tx;
        y0 = ty;
        
    }
    
    dx = abs(x1 - x0);
    dy = abs(y1 - y0);
    
    if(dx >= dy){
        de =  2 * dy - dx;
        if(dx >= dy && x0 <= x1){
            c = dx;
            x = x0;
            y = y0;
        }else{
            c = dx;
            x = x1;
            y = y1;
        }
        
        for(U32 i = 0; i < c * resolution; i++){
            x += 1;
            DrawPixel(x, y, color);
            if(de > 0){
                if((x0 < x1 && y0 < y1) || (x0 > x1 && y0 > y1)) y += 1; else y -= 1;
                de -= 2*dx;
            }
            de += 2*dy;
        }
    }else{
        if(dx <= dy){
            de = 2 * dx - dy;
            if(dy >= dx && y0 <= y1){
                c = dy;
                x = x0;
                y = y0;
            }else{
                c = dy;
                x = x1;
                y = y1;
            }
            for(U32 i = 0; i < c * resolution; i++){
                y += 1;
                DrawPixel(x, y, color);
                if(de >= 0){
                    if((x0 < x1 && y0 < y1) || (x0 > x1 && y0 > y1)) x += 1; else x -= 1;
                    de -= 2*dy;
                }
                de += 2*dx;
            }
            
        }
    }
}


