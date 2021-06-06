void SetUp();
void SelectColor();
void SelectMapPixel();
void Save();
void Load();
void Draw();
U8 *CreateHeaderBMP(U32 width, U32 height, U32 size);
//void ExportBMP(TILEINFO *bitmap, U8 *name);
void ExportMapBMP(MAPINFO *bitmap);
//void ImportBMP();
void ImportMapBMP();
void Quit();

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
    eraser.x = 15;
    eraser.y = 1;
    
    
    for(U8 i = 0; i < 8; i++){
        for(U8 j = 0; j < 10; j++){
            sprite_map[i * 10 + j].x = j + 1;
            sprite_map[i * 10 + j].y = i + 10;
        }
    }
    
    
    for(U32 z = 0; z < 80; z++){
        for(U32 i = 0; i < 8; i++){
            for(U32 j = 0; j < 8; j++){
                sprite_map[z].pixel[i * 8 + j].x = j;
                sprite_map[z].pixel[i * 8 + j].y = i;
            }
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
    eraser.color = STD_DARK_GREY;
    
    for(U8 i = 0; i < 64; i++){
        drawboard[i].color = STD_DARK_GREY;
    }
    for(U32 i = 0; i < 80; i++){
        for(U32 j = 0; j < 64; j++){
            sprite_map[i].pixel[j].color = STD_DARK_GREY;
        }
    }
    
    highlight_x = 0;
    highlight_y = 0;
    
    editor_font = LoadSpriteMap("mc_font.bmp");
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
    
    if(button(LBUTTON).changed == 1 && display_mouse.x == 15 && display_mouse.y == 1){
        display_mouse.color = eraser.color;
        highlight_x = display_mouse.x;
        highlight_y = display_mouse.y;
    }
}

void SelectMapPixel(){
    if(button(LBUTTON).changed == 1 && display_mouse.x > 0 && display_mouse.x < 11 && display_mouse.y > 9 && display_mouse.y < 18){
        tile_selected = (display_mouse.y * 10 + display_mouse.x) - 101;
        for(U32 i = 0; i < 64; i++){
            drawboard[i].color = sprite_map[tile_selected].pixel[i].color;
        }
    }
    
    for(U32 i = 0; i < 64; i++){
        sprite_map[tile_selected].pixel[i].color = drawboard[i].color;
    }
}

void Save(){
    if(button(LBUTTON).changed == 1 && display_mouse.x > 15 && display_mouse.x < 25 && display_mouse.y > 2 && display_mouse.y < 6){
        save.cursor_active = 1;
    }
    
    if(button(LBUTTON).changed == 1 && (display_mouse.x < 16 ||  display_mouse.x > 24 || display_mouse.y < 3 || display_mouse.y > 5)){
        save.cursor_active = 0;
    }
    if(save.cursor_active == 1){ 
        if(save.cursor_pos < 10 && TypeKey() != 0){ 
            save.arr_name[save.cursor_pos] = TypeKey();
            save.cursor_pos++;
        }
        if(pressed(BACKSPACE) && save.cursor_pos > 0){
            save.arr_name[save.cursor_pos] = 0;
            save.cursor_pos--;
            save.arr_name[save.cursor_pos] = 0;
        }
        if(save.cursor_pos < 10) save.arr_name[save.cursor_pos] = '_';
        
    }
    if(save.cursor_active == 0){
        save.arr_name[save.cursor_pos] = ' ';
    }
    
    if( button(LBUTTON).changed == 1 && display_mouse.x > 15 && display_mouse.x < 21 && display_mouse.y == 5){
        save.color = YELLOW;
    } else save.color = ORANGE;
}

void Load(){
    
    if(button(LBUTTON).changed == 1 && display_mouse.x > 15 && display_mouse.x < 25 && display_mouse.y > 7 && display_mouse.y < 11){
        load.cursor_active = 1;
    }
    
    if(button(LBUTTON).changed == 1 && (display_mouse.x < 16 ||  display_mouse.x > 24 || display_mouse.y < 8 || display_mouse.y > 10)){
        load.cursor_active = 0;
    }
    if(load.cursor_active == 1){ 
        if(load.cursor_pos < 10 && TypeKey() != 0){ 
            load.arr_name[load.cursor_pos] = TypeKey();
            load.cursor_pos++;
        }
        if(pressed(BACKSPACE) && load.cursor_pos > 0){
            load.arr_name[load.cursor_pos] = 0;
            load.cursor_pos--;
            load.arr_name[load.cursor_pos] = 0;
        }
        if(load.cursor_pos < 10) load.arr_name[load.cursor_pos] = '_';
        
    }
    if(load.cursor_active == 0){
        load.arr_name[load.cursor_pos] = ' ';
    }
    
    if( button(LBUTTON).changed == 1 && display_mouse.x > 15 && display_mouse.x < 21 && display_mouse.y == 10){
        load.color = DARK_PURPLE;
    } else load.color = DARK_BLUE;
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
    FillRectangle(eraser.x*SIZE, eraser.y*SIZE, SIZE, SIZE, eraser.color);
    
    //DRAW SPRITE_MAP
    for(U8 i = 0; i < 80; i++){
        for(U8 j = 0; j < 64; j++){
            FillRectangle((sprite_map[i].x * SIZE) + (sprite_map[i].pixel[j].x * SIZE/8), (sprite_map[i].y * SIZE) + (sprite_map[i].pixel[j].y * SIZE/8), SIZE/8, SIZE/8, sprite_map[i].pixel[j].color);
        }
    }
    
    //COLOR HIGHLIGHT
    DrawRectangle(highlight_x * SIZE - 1, highlight_y * SIZE - 1, SIZE + 2, SIZE + 2, 2, display_mouse.color);
    
    if(display_mouse.x > 0 && display_mouse.x < 9  && display_mouse.y > 0 && display_mouse.y < 9){
        if(input.key[LBUTTON].changed == 1){
            drawboard[(display_mouse.y - 1) * 8 + (display_mouse.x - 1)].color = display_mouse.color;
        }
    }
    
    
    //DRAW COORDS
    for(U8 i = 0; i < 8; i++) DrawSprite(1 + i, 0, 60 + i, editor_font);
    for(U8 i = 0; i < 8; i++) DrawSprite(0, 1 + i, 60 + i, editor_font);
    for(U8 i = 0; i < 10; i++) DrawSprite(1 + i, 9, 60 + i, editor_font);
    for(U8 i = 0; i < 8; i++) DrawSprite(0, 10 + i, 60 + i, editor_font);
    
    //DRAW LINES
    if(lines == 0){
        //horizontal
        for(U8 i = 0; i < 9; i++) FillRectangle(1 * SIZE, (1 + i) * SIZE, 8 * SIZE, 1, WHITE);
        for(U8 i = 0; i < 9; i++)  FillRectangle(1 * SIZE, (10 + i) * SIZE, 10 * SIZE, 1, WHITE);
        //vertical
        for(U8 i = 0; i < 9; i++) FillRectangle((1 + i) * SIZE, 1 * SIZE, 1, 8 * SIZE + 1, WHITE);
        for(U8 i = 0; i < 11; i++) FillRectangle((1 + i) * SIZE, 10 * SIZE, 1, 8 * SIZE + 1, WHITE);
    }
    //HIGHLIGHT MAP SELECTION
    DrawRectangle(sprite_map[tile_selected].x * SIZE, sprite_map[tile_selected].y * SIZE, SIZE + 1, SIZE + 1, 1, PURPLE);
    
    //MAP NAMES
    Print(16, 3, editor_font, "MAP NAME", WHITE);
    if(capslock == 1) Print(23, 3, editor_font, "C", RED);
    FillRectangle(16 * SIZE, 4 * SIZE, SIZE * 8, SIZE, STD_DARK_GREY);
    Print(16, 4, editor_font, save.arr_name, WHITE);
    Print(16, 5, editor_font, "SAVE", save.color);
    
    Print(16, 8, editor_font, "LOAD NAME", WHITE);
    if(capslock == 1) Print(23, 8, editor_font, "C", RED);
    FillRectangle(16 * SIZE, 9 * SIZE, SIZE * 8, SIZE, STD_DARK_GREY);
    Print(16, 9, editor_font, load.arr_name, WHITE);
    Print(16, 10, editor_font, "LOAD", load.color);
}

U8 *CreateHeaderBMP(U32 width, U32 height, U32 size){
    static U8 header[54] = {0};
    
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
    
    return header;
}

/*void ExportBMP(TILEINFO *bitmap, U8 *name){
    printf("Creating BMP\n");
    U8 *header = CreateHeaderBMP(8, 8, 64);
    
    U8 map[64*4];
    U8 reversed  = 7;
    
    for(U8 i = 0; i < 8; i++){
        for(U8 j = 0; j < 8; j++){
            U32 p = (reversed * 8 + j) * 4;
            map[p] = (U8)drawboard[i*8+j].color;
            map[p+1] = (U8)(drawboard[i*8+j].color >> 8);
            map[p+2] = (U8)(drawboard[i*8+j].color >> 16);
            map[p+3] = 0x00;
        }
        reversed--;
    }
    
    
    FILE *file = fopen(name, "wb");
    fwrite(header, 1, 54, file);
    fwrite(map, 1, 64*4, file);
    fclose(file);
}*/

void ExportMapBMP(MAPINFO *bitmap){
    printf("Creating Map BMP\n");
    U8 *header = CreateHeaderBMP(80, 64, 80 * 64 * 4);
    U8 name[16];
    U8 i = 0;
    
    U8 map[80 * 64 * 4];
    for(U8 k = 0; k < 80; k++){
        U8 reversed = 7;
        U32 offset = 80 * 64 - (80 * (7 * (bitmap[k].y - 9)) + (80 - 8 * (bitmap[k].x - 1)) + (80 * (bitmap[k].y - 10))); // width * nºlines + offset last line + offset 1 line
        for(U8 i = 0; i < 8; i++){
            for(U8 j = 0; j < 8; j++){
                U32 p = ((i * 8 + j) + offset) * 4;
                map[p] = (U8) bitmap[k].pixel[reversed*8+j].color;
                map[p + 1] = (U8)(bitmap[k].pixel[reversed*8+j].color >> 8);
                map[p + 2] = (U8)(bitmap[k].pixel[reversed*8+j].color >> 16);
                map[p + 3] = 0x00;
            }
            offset += 72;
            reversed--;
        }
    }
    
    while(save.arr_name[i] != '\0'){
        name[i] = save.arr_name[i];
        if(name[i] == '_' && save.arr_name[i + 1] == '\0'){
            name[i] = '.';
            name[i + 1] = 'b';
            name[i + 2] = 'm';
            name[i + 3] = 'p';
            break;
        }
        if(i == 9){
            name[i + 1] = '.';
            name[i + 2] = 'b';
            name[i + 3] = 'm';
            name[i + 4] = 'p';
            break;
        }
        i++;
    }
    
    FILE *file = fopen(name, "wb");
    fwrite(header, 1, 54, file);
    fwrite(map, 1, 80 * 64 * 4, file);
    fclose(file);
}

/*void ImportBMP(){
    
    U8 map[64*4];
    
    FILE *file =  fopen("drawboard.bmp", "r");
    fseek(file, 54, SEEK_SET);
    fread(map, 1, 64*4, file);
    fclose(file);
    
    U32 reversed = 7;
    for(U32 i = 0; i < 8; i++){
        for(U32 j = 0; j < 8; j++){
            U32 p = (reversed * 8 + j) * 4;
            drawboard[i*8+j].color = (map[p+2] << 16)|(map[p+1] << 8)|map[p];
        }
        reversed--;
    }
    printf("BMP Loaded!\n");
}*/

void ImportMapBMP(){
    
    U8 map[80 * 64 * 4];
    U8 name[16] = {0};
    U8 i = 0;
    
    printf("\n");
    while(load.arr_name[i] != '\0'){
        name[i] = load.arr_name[i];
        if(name[i] == '_' && load.arr_name[i + 1] == '\0'){
            name[i] = '.';
            name[i + 1] = 'b';
            name[i + 2] = 'm';
            name[i + 3] = 'p';
            break;
        }
        if(i == 9){
            name[i + 1] = '.';
            name[i + 2] = 'b';
            name[i + 3] = 'm';
            name[i + 4] = 'p';
            break;
        }
        i++;
    }
    
    FILE *file = fopen(name, "r");
    fseek(file, 54, SEEK_SET);
    fread(map, 1, 80 * 64 * 4, file);
    fclose(file);
    
    for(U8 k = 0; k < 80; k++){
        U32 offset = 80 * 64 - (80 * (7 * (sprite_map[k].y - 9)) + (80 - 8 * (sprite_map[k].x - 1)) + (80 * (sprite_map[k].y - 10)));
        U8 reversed = 7;
        for(U8 i = 0; i < 8; i++){
            for(U8 j = 0; j < 8; j++){
                U32 p = ((i * 8 + j) + offset) * 4;
                sprite_map[k].pixel[reversed*8+j].color = (map[p+2] << 16)|(map[p+1] << 8)|map[p];
            }
            offset += 72;
            reversed--;
        }
    }
    printf("Texture Map Loaded!\n");
    
    //fix texture map in relation with drawboard
    for(U8 i = 0; i < 64; i++){
        drawboard[i].color = sprite_map[tile_selected].pixel[i].color;
    }
    
}

void Quit(){
    free(editor_font);
}