void SetUp();
void Draw();
void ExportBMP(TILEINFO *bitmap, U32 width, U32 height, U8 *name);
void ImportBMP();

void SetUp(){
    
    //TILEINFO POSITION
    for(U8 i = 0; i < 8; i++){
        for(U8 j = 0; j < 8; j++){
            drawboard[i * 8 + j].x = j + 1;
            drawboard[i * 8 + j].y = i + 1;
        }
    }
    
    for(U8 i = 0; i < 4; i++){
        for(U8 j = 0; j < 4; j++){
            colors[i * 4 + j].x = j + 11;
            colors[i * 4 + j].y = i + 1;
        }
    }
    
    for(U32 z = 0; z < 80; z++){
        for(U32 i = 0; i < 8; i++){
            for(U32 j = 0; j < 8; j++){
                sprite_map[z].pixel[i * 8 *j].x = j;
                sprite_map[z].pixel[i * 8 *j].y = i;
            }
        }
    }
    
    /*for(U8 i = 0; i < 8; i++){
        for(U8 j = 0; j < 10; j++){
            sprite_map[i * 10 + j].x = j + 1;
            sprite_map[i * 10 + j].y = i + 10;
        }
    }*/
    
    for(U32 z = 0; z < 80; z++){
        for(U32 c = 0; c < 64; c++){
            printf("%d %d\n", sprite_map[z].pixel[c].x, sprite_map[z].pixel[c].y);
        }
    }
    
    //TILEINFO PALETTE
    colors[0].color = BLACK;
    colors[1].color = DARK_BLUE;
    colors[2].color = DARK_PURPLE;
    colors[3].color = DARK_GREEN;
    colors[4].color = BROWN;
    colors[5].color = DARK_GREY;
    colors[6].color = LIGHT_GREY;
    colors[7].color = WHITE;
    colors[8].color = RED;
    colors[9].color = ORANGE;
    colors[10].color = YELLOW;
    colors[11].color = GREEN;
    colors[12].color = BLUE;
    colors[13].color = LAVANDER;
    colors[14].color = PINK;
    colors[15].color = LIGHT_PEACH;
    
    for(U8 i = 0; i < 64; i++){
        drawboard[i].color = BLACK;
    }
    for(U32 i = 0; i < 80; i++){
        for(U32 j = 0; j < 64; j++){
            sprite_map[i].pixel[j].color = BLACK;
        }
    }
    
    highlight_x = 0;
    highlight_y = 0;
    
}

void SelectColor(){
    if(bar_placement == 1){ 
        clamp(black_bar, input.mouse.y, draw_surface.height + black_bar);
        display_mouse.y = (input.mouse.y - black_bar)/(SIZE*resolution);
        display_mouse.x = input.mouse.x/(SIZE*resolution);
    }
    if(bar_placement == 2){ 
        clamp(black_bar, input.mouse.x, draw_surface.width + black_bar);
        display_mouse.x = (input.mouse.x - black_bar)/(SIZE*resolution);
        display_mouse.y = input.mouse.y/(SIZE*resolution);
    }
    
    if(button(LBUTTON).changed == 1 && display_mouse.x > 10 && display_mouse.x < 15 && display_mouse.y > 0 && display_mouse.y < 5){
        display_mouse.color = colors[(display_mouse.y - 1) * 4 + (display_mouse.x - 11)].color;
        highlight_x = display_mouse.x;
        highlight_y = display_mouse.y;
    }
}

void Draw(){
    FillScreen(DARK_GREY);
    
    //DRAW DRAWBOARD
    for(U8 i = 0; i < 64; i++){
        FillRectangle(drawboard[i].x*SIZE, drawboard[i].y*SIZE, SIZE, SIZE, drawboard[i].color);
    }
    
    //DRAW PALETTE
    for(U8 i = 0; i < 16; i++){
        FillRectangle(colors[i].x*SIZE, colors[i].y*SIZE, SIZE, SIZE, colors[i].color);
    }
    
    //DRAW SPRITE_MAP
    for(U8 i = 0; i < 80; i++){
        for(U8 j = 0; j < 64; j++){
            FillRectangle(sprite_map[i].x, sprite_map[i].y, SIZE/8, SIZE/8, sprite_map[i].pixel[j].color);
        }
    }
    
    //COLOR HIGHLIGHT
    DrawRectangle(highlight_x * SIZE - 1, highlight_y * SIZE - 1, SIZE + 2, SIZE + 2, 10, display_mouse.color);
    
    if(display_mouse.x > 0 && display_mouse.x < 9  && display_mouse.y > 0 && display_mouse.y < 9){
        if(input.key[LBUTTON].changed == 1){
            drawboard[(display_mouse.y - 1) * 8 + (display_mouse.x - 1)].color = display_mouse.color;
        }
    }
    
}

void ExportBMP(TILEINFO *bitmap, U32 width, U32 height, U8 *name){
    printf("Creating BMP\n");
    
    U8 header[54] = {0};
    U32 size = width * height * 4; 
    
    header[0] = 'B';
    header[1] = 'M';
    memset(&header[2], 54 + size, 1);
    header[10] = 54;
    header[14] = 40;
    memset(&header[18], width, 1);
    memset(&header[22], height, 1);
    header[26] = 1;
    header[27] = 0;
    header[28] = 32;
    memset(&header[34], size, 1);
    
    U8 map[size];
    U8 reversed  = height - 1;
    
    for(U32 i = 0; i < height; i++){
        for(U32 j = 0; j < width; j++){
            U32 p = (reversed * width + j) * 4;
            map[p] = (U8)drawboard[i*width+j].color;
            map[p+1] = (U8)(drawboard[i*width+j].color >> 8);
            map[p+2] = (U8)(drawboard[i*width+j].color >> 16);
            map[p+3] = 0x00;
            
        }
        reversed--;
    }
    
    
    FILE *file = fopen(name, "wb");
    fwrite(header, 1, 54, file);
    fwrite(map, 1, size, file);
    free(header);
    fclose(file);
}

void ImportBMP(){
    
    U32 width = 8;
    U32 height = 8;
    U32 reversed = height - 1;
    U32 size = width * height * 4;
    U8 map[size];
    FILE *file =  fopen("mybmp.bmp", "r");
    fseek(file, 54, SEEK_SET);
    fread(map, 1, size, file);
    fclose(file);
    
    for(U32 i = 0; i < height; i++){
        for(U32 j = 0; j < width; j++){
            U32 p = (reversed * width + j) * 4;
            drawboard[i*width+j].color = (map[p+2] << 16)|(map[p+1] << 8)|map[p];
        }
        reversed--;
    }
    printf("BMP Loaded!\n");
}