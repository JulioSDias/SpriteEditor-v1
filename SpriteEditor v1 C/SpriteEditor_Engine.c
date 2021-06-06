void DrawPixel(U32 x, U32 y, I32 color);
void FillScreen(I32 color);
void FillRectangle(U32 x, U32 y, U32 width, U32 height, I32 color);
void DrawRectangle(U32 x, U32 y, U32 width, U32 height, U32 wide, I32 color);
TILEINFO *LoadSprite(U8 *name);
MAPINFO *LoadSpriteMap(U8 *name);
void DrawSprite(U32 x, U32 y, U32 pos, MAPINFO *map);
void DrawLine(U32 x0, U32 y0, U32 x1, U32 y1, I32 color);
void Print(U32 x, U32 y, MAPINFO *font, U8 *phrase, U32 color);
U8 TypeKey();

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

TILEINFO *LoadSprite(U8 *name){
    TILEINFO *sprite;
    
    sprite = (TILEINFO *) malloc(sizeof(TILEINFO) * 64);
    
    U8 map[64*4];
    U8 reversed = 7;
    FILE *file = fopen(name, "r");
    fseek(file, 54, SEEK_SET);
    fread(map, 1, 64*4, file);
    fclose(file);
    for(U8 i = 0; i < 8; i++){
        for(U8 j = 0; j < 8; j++){
            U32 p = (reversed * 8 + j) * 4;
            sprite[i*8+j].color = (map[p+2] << 16)|(map[p+1] << 8)|map[p];
            sprite[i*8+j].x = j;
            sprite[i*8+j].y = i;
        }
        reversed--;
    }
    
    return sprite;
}

MAPINFO *LoadSpriteMap(U8 *name){
    MAPINFO *sprite;
    
    sprite = (MAPINFO *) malloc(sizeof(MAPINFO) * 80);
    
    for(U8 i = 0; i < 8; i++){
        for(U8 j = 0; j < 10; j++){
            sprite[i * 10 + j].x = j;
            sprite[i * 10 + j].y = i;
        }
    }
    
    U8 map[80*64*4];
    FILE *file = fopen(name, "r");
    fseek(file, 54, SEEK_SET);
    fread(map, 1, 80*64*4, file);
    fclose(file);
    for(U8 k = 0; k < 80; k++){
        U8 reversed = 7;
        U32 offset = 80*64 - (80 * (7 * (sprite[k].y + 1)) + (80 - 8 * (sprite[k].x)) + (80 * (sprite[k].y)));
        for(U8 i = 0; i < 8; i++){
            for(U8 j = 0; j < 8; j++){
                U32 p = ((i * 8 + j) + offset) * 4;
                sprite[k].pixel[reversed*8+j].color = (map[p+2] << 16)|(map[p+1] << 8)|map[p];
            }
            offset += 72;
            reversed--;
        }
    }
    return sprite;
}


void DrawSprite(U32 x, U32 y, U32 pos, MAPINFO *map){
    for(U8 i = 0; i < 8; i++){
        for(U8 j = 0; j < 8; j++){
            map[pos].pixel[i * 8 + j].x = j;
            map[pos].pixel[i * 8 + j].y = i;
            if(map[pos].pixel[i * 8 + j].color != STD_DARK_GREY){
                FillRectangle((map[pos].pixel[i * 8 + j].x * SIZE/8) + (x * SIZE),  (map[pos].pixel[i * 8 + j].y * SIZE/8) + (y * SIZE), SIZE/8, SIZE/8,  map[pos].pixel[i * 8 + j].color);
            }
        }
    }
}

void Print(U32 x, U32 y, MAPINFO *font, U8 *phrase, U32 color){
    U8 k = 0;
    while(phrase[k] != '\0'){
        for(U8 h  = 0; h < 80; h++){
            if(chars[h] == phrase[k]){
                for(U8 i = 0; i < 8; i++){
                    for(U8 j = 0; j < 8; j++){
                        font[h].pixel[i * 8 + j].x = j;
                        font[h].pixel[i * 8 + j].y = i;
                        if(font[h].pixel[i * 8 + j].color != STD_DARK_GREY){
                            FillRectangle((font[h].pixel[i * 8 + j].x * SIZE/8) + (x * SIZE) + (k * 12),  (font[h].pixel[i * 8 + j].y * SIZE/8) + (y * SIZE), SIZE/8, SIZE/8,  color);
                        }
                    }
                }
                k++;
            }
        }
    }
}

U8 TypeKey(){
    
    if(hold(SHIFT)) capslock = 1;
    if(released(SHIFT))capslock = 0;
    if(pressed(CAPSLOCK))capslock = !capslock;
    if(pressed(SPACEBAR)) return ' ';
    if(pressed(SUBTRACT) && !hold(SHIFT)) return '-';
    if(pressed(SUBTRACT) && hold(SHIFT)) return '_';
    if(pressed(PERIOD) && !hold(SHIFT)) return '.';
    if(pressed(PERIOD) && hold(SHIFT)) return ':';
    if(pressed(COMMA) && !hold(SHIFT)) return ',';
    if(pressed(COMMA) && hold(SHIFT)) return ';';
    if(pressed(QUESTION)) return '?';
    if(pressed(ZERO)) return '0';
    if(pressed(ONE) && !hold(SHIFT)) return '1';
    if(pressed(ONE) && hold(SHIFT)) return '!';
    if(pressed(TWO)) return '2';
    if(pressed(THREE)) return '3';
    if(pressed(FOUR) && !hold(SHIFT)) return '4';
    if(pressed(FOUR) && hold(SHIFT)) return '$';
    if(pressed(FIVE)) return '5';
    if(pressed(SIX)) return '6';
    if(pressed(SEVEN)) return '7';
    if(pressed(EIGHT) && !hold(SHIFT)) return '8';
    if(pressed(EIGHT) && hold(SHIFT)) return '(';
    if(pressed(NINE) && !hold(SHIFT)) return '9';
    if(pressed(NINE) && hold(SHIFT)) return ')';
    if(pressed(A) && capslock == 0) return 'a';
    if(pressed(B) && capslock == 0) return 'b';
    if(pressed(C) && capslock == 0) return 'c';
    if(pressed(D) && capslock == 0) return 'd';
    if(pressed(E) && capslock == 0) return 'e';
    if(pressed(F) && capslock == 0) return 'f';
    if(pressed(G) && capslock == 0) return 'g';
    if(pressed(H) && capslock == 0) return 'h';
    if(pressed(I) && capslock == 0) return 'i';
    if(pressed(J) && capslock == 0) return 'j';
    if(pressed(K) && capslock == 0) return 'k';
    if(pressed(L) && capslock == 0) return 'l';
    if(pressed(M) && capslock == 0) return 'm';
    if(pressed(N) && capslock == 0) return 'n';
    if(pressed(O) && capslock == 0) return 'o';
    if(pressed(P) && capslock == 0) return 'p';
    if(pressed(Q) && capslock == 0) return 'q';
    if(pressed(R) && capslock == 0) return 'r';
    if(pressed(S) && capslock == 0) return 's';
    if(pressed(T) && capslock == 0) return 't';
    if(pressed(U) && capslock == 0) return 'u';
    if(pressed(V) && capslock == 0) return 'v';
    if(pressed(W) && capslock == 0) return 'w';
    if(pressed(X) && capslock == 0) return 'x';
    if(pressed(Y) && capslock == 0) return 'y';
    if(pressed(Z) && capslock == 0) return 'z';
    if(pressed(A) && capslock == 1) return 'A';
    if(pressed(B) && capslock == 1) return 'B';
    if(pressed(C) && capslock == 1) return 'C';
    if(pressed(D) && capslock == 1) return 'D';
    if(pressed(E) && capslock == 1) return 'E';
    if(pressed(F) && capslock == 1) return 'F';
    if(pressed(G) && capslock == 1) return 'G';
    if(pressed(H) && capslock == 1) return 'H';
    if(pressed(I) && capslock == 1) return 'I';
    if(pressed(J) && capslock == 1) return 'J';
    if(pressed(K) && capslock == 1) return 'K';
    if(pressed(L) && capslock == 1) return 'L';
    if(pressed(M) && capslock == 1) return 'M';
    if(pressed(N) && capslock == 1) return 'N';
    if(pressed(O) && capslock == 1) return 'O';
    if(pressed(P) && capslock == 1) return 'P';
    if(pressed(Q) && capslock == 1) return 'Q';
    if(pressed(R) && capslock == 1) return 'R';
    if(pressed(S) && capslock == 1) return 'S';
    if(pressed(T) && capslock == 1) return 'T';
    if(pressed(U) && capslock == 1) return 'U';
    if(pressed(V) && capslock == 1) return 'V';
    if(pressed(W) && capslock == 1) return 'W';
    if(pressed(X) && capslock == 1) return 'X';
    if(pressed(Y) && capslock == 1) return 'Y';
    if(pressed(Z) && capslock == 1) return 'Z';
    return 0;
}

//doesnt work with res
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



