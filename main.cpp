#include <iostream>
#include <math.h>
#include <sl.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <cstring>

using namespace std;

void Load_Options(string _file_Name),
    Load_UI(string _file_Name),
    Load_Board(string _file_Name),
    Calculate_Board_HitBox(),
    Create_Matrix(),
    Draw_Board(),
    Draw_UI(),
    UI_Decoder(string _tag),
    Game_Window(),
    Deselect_Piece(),
    Undo(),
    Wait(double _delay),
    Random_Solve();

void Bkt_Solve();
void Select_Piece();
void Deselect_Piece();

//Vector2 is used for storing both coordinates and dimensions
struct Vector2
{

    int x = 0,y = 0;

};

//Game_Settings_Preset is used for storing the settings of the games
struct Game_Setting_Preset
{

    Vector2 Size,Ratio;
    int UI_Scalar_Unit;
    bool English, Fullscreen = false;
    string Piece_Color;


};

//The Pattern describes the cut-off areas of the board
struct Pattern
{

    Vector2 Step_Size;
    int Step_Start_Number,Step_Decline;

};

//Texture stores the file location of a sprite's texture
struct Texture
{

    int Texture;
    string Texture_Adr;

};

//UI_Element correlates an area of the window with a code, that is later correlated with a function
struct UI_Element
{

    string Center_Alignment = "LD", Function_Tag, Displayed_String_EN, Displayed_String_RO, Window_UI_Tag;
    Vector2 Center, Dimensions, Position, Actual_Dimensions;
    int Scale_Modifier = 1;
    Texture UI_Element_Texture;

};

//Score_Unit stores the initials, score and time
struct Score_Unit
{

    string Initial = "";
    int Score = 0;
    int Time = 0;

};

//Board stores the values of the game board
struct Board
{

    int * Matrix;
    Vector2 Size, Empty_Tile;
    int Tile_Size, Tile_Margine, Code = 0, Current_Score_Rank = 5;
    Pattern Board_Pattern;
    int UI_Vector_Position;
    double Time = 0;
    Score_Unit Current_Score_Board[6], Score_Board[5];

};

//This struct describes a move on the board in bkt
struct Move
{
    int Start,Middle,End;
} Solution[255],Temporary[255];

int Best,Step;

//Selected_Tile holds the position of the selected tile in the matrix
//No_Of_Pieces stores the starting number of pieces on a board
//Current_Board_Index holds the currently selected board
int Selected_Tile = -1, No_Of_Pieces = 0, Current_Board_Index = 1;

//These two arrays hold the board for the menu and game
int Main_Board_Int_Array[225], Menu_Board_Int_Array[225];

//Key_Char holds the last pressed Key
char Key_Char;

//Current_Window_UI_Tag holds the string key that enables the UI_Elements corresponding to the appropriate window
//Game - displays the game window
//Main_Menu - displays the main menu of the game
//Game_Select_Menu - displays the board select menu
//Main_Options_Menu - displays the options menu of the game
//Last_Key_Pressed holds the input from the keyboard
string Current_Window_UI_Tag = "Main_Menu", Last_Key_Pressed;

//The Board_Address_Array stores the addresses of the board files
string Board_Address_Array[13] = {"files/Cross_Show_Board.txt", "files/7x7_Cross.txt", "files/9x9_Cross.txt", "files/11x11_Cross.txt", "files/13x13_Cross.txt", "files/7x7_Corner.txt", "files/9x9_Corner.txt", "files/11x11_Corner.txt", "files/13x13_Corner.txt", "files/5x9_Extra.txt", "files/7x7_Extra.txt", "files/9x9_Extra.txt", "files/11x11_Extra.txt"};

//These bools are used for holding the different game states
bool End_Game = true, End_Show_Game = false, Window_Closed = false, Game_Board_From_File = false, Enable_Undo = false, Show_Close_Window_Dialogue = false, Take_Input_From_Keyboard = true, Has_Key_Been_Pressed = false, Has_Written_To_File = false;

//Game_Board holds the variables related to the board
//Temp_Game_Board temporarily holds the variables of Game_Board
Board Game_Board, Temp_Game_Board;

//Temp_UI_Board_Element holds the variables of the corresponding UI_Element of the game board
UI_Element Temp_UI_Board_Element;

//Window_UI_Vector stores the UI_Elements corresponding to each button
vector<UI_Element> Window_UI_Vector;

//Game_Settings stores the settings of the game
//Temp_Game_Settings holds the temporary settings of the game
Game_Setting_Preset Game_Settings, Temp_Game_Settings;

//This function returns the input from the mouse and keyboard
string Return_Key()
{

    string _temp_String = "";

    if(!Has_Key_Been_Pressed)
    {

        if(slGetMouseButton(SL_MOUSE_BUTTON_LEFT))
        {

            _temp_String = "<SL_MOUSE_BUTTON_LEFT>";

        }
        else if (slGetKey(SL_KEY_ENTER))
        {

            _temp_String = "<SL_KEY_ENTER>";

        }
        else if(slGetKey(SL_KEY_BACKSPACE))
        {

            _temp_String = "<SL_KEY_BACKSPACE>";

        }
        else
        {

            for(char i = 'A'; i <= 'Z'; i++)
            {

                if(slGetKey(i))
                {

                    Key_Char = i;
                    _temp_String = i;
                    break;

                }

            }

        }

        if(_temp_String != "")
        {

            Has_Key_Been_Pressed = true;

        }

    }
    else
    {


        if(!slGetMouseButton(SL_MOUSE_BUTTON_LEFT) && !slGetKey(SL_KEY_ENTER) && !slGetKey(Key_Char))
        {

            Has_Key_Been_Pressed = false;

        }

        _temp_String = "";

    }

    return _temp_String;
}

//Returns the value between the tags
string Return_Value(string _row, string _tag)
{

    int temp_pos = _row.find("<"+_tag+">");

    if(temp_pos != UINT_MAX)
    {

        _row.erase(temp_pos, temp_pos + _tag.length() + 2);

    }

    temp_pos = _row.find("</"+_tag+">");

    if(temp_pos != UINT_MAX)
    {

        _row.erase(temp_pos, temp_pos + _tag.length() + 3);

    }

    return _row;

}

//Checks if the string contains the tag
bool String_Contains(string _row, string _tag)
{

    if(_row.find("<"+_tag+">") != UINT_MAX)
    {

        return true;

    }
    else
    {

        return false;

    }


}

//Writes/Replaces a value in the given file
void Write_Value(string _file, string _tag, string _value)
{

    string _temp_String = "<" + _tag + ">" + _value + "</" + _tag + ">"+"\n";

    bool _has_Written_The_Value = 0;

    int _nr_Of_Rows = 0;

    FILE *_temp_File;
    _temp_File = fopen("files/_temp.txt", "r+");

    FILE *_file_To_Write;
    _file_To_Write = fopen(_file.c_str(), "r+");

    char _temp_Char_Array[100];

    if(_temp_File == NULL)
    {
        perror("Error!");
        exit(1);
    }
    if(_file_To_Write == NULL)
    {
        perror("Error!");
        exit(1);
    }

    fseek(_temp_File, 0, SEEK_SET);
    fseek(_file_To_Write, 0, SEEK_SET);

    while(fgets (_temp_Char_Array, 100, _file_To_Write) != NULL)
    {

        fputs(_temp_Char_Array, _temp_File);

        _nr_Of_Rows++;

    }

    fseek(_temp_File, 0, SEEK_SET);
    fseek(_file_To_Write, 0, SEEK_SET);

    for(int i = 0; fgets (_temp_Char_Array, 100, _temp_File) != NULL && i < _nr_Of_Rows; i++)
    {

        if(String_Contains(_temp_Char_Array, _tag) && !_has_Written_The_Value)
        {

            fputs(_temp_String.c_str(), _file_To_Write);

            _has_Written_The_Value = true;

        }
        else
        {

            fputs(_temp_Char_Array, _file_To_Write);

        }

    }

    if(!_has_Written_The_Value)
    {

        fputs(_temp_String.c_str(), _file_To_Write);

        _has_Written_The_Value = true;

    }

    fclose(_temp_File);
    fclose(_file_To_Write);

}

//Translates the coordinates and alignment to the real set of coordinates
Vector2 UI_Element_Alignment (Vector2 _position, int _scale, string _alignment)
{

    Vector2 _temp;

    if (_alignment == "C")
    {

        _temp.x = Game_Settings.Size.x/2 + _position.x*Game_Settings.UI_Scalar_Unit/_scale;
        _temp.y = Game_Settings.Size.y/2 + _position.y*Game_Settings.UI_Scalar_Unit/_scale;

    }
    else if (_alignment == "TL")
    {

        _temp.x = _position.x*Game_Settings.UI_Scalar_Unit/_scale;
        _temp.y = Game_Settings.Size.y + _position.y*Game_Settings.UI_Scalar_Unit/_scale;

    }
    else if (_alignment == "TR")
    {

        _temp.x = Game_Settings.Size.x + _position.x*Game_Settings.UI_Scalar_Unit/_scale;
        _temp.y = Game_Settings.Size.y + _position.y*Game_Settings.UI_Scalar_Unit/_scale;

    }
    else if (_alignment == "DR")
    {

        _temp.x = Game_Settings.Size.x + _position.x*Game_Settings.UI_Scalar_Unit/_scale;
        _temp.y = _position.y*Game_Settings.UI_Scalar_Unit/_scale;

    }
    else if (_alignment == "LC")
    {

        _temp.x = _position.x*Game_Settings.UI_Scalar_Unit/_scale;
        _temp.y = Game_Settings.Size.y/2 + _position.y*Game_Settings.UI_Scalar_Unit/_scale;

    }
    else if (_alignment == "RC")
    {

        _temp.x = Game_Settings.Size.x + _position.x*Game_Settings.UI_Scalar_Unit/_scale;
        _temp.y = Game_Settings.Size.y/2 + _position.y*Game_Settings.UI_Scalar_Unit/_scale;

    }
    else if (_alignment == "DC")
    {

        _temp.x = Game_Settings.Size.x/2 + _position.x*Game_Settings.UI_Scalar_Unit/_scale;
        _temp.y = _position.y*Game_Settings.UI_Scalar_Unit/_scale;

    }
    else if (_alignment == "TC")
    {

        _temp.x = Game_Settings.Size.x/2 + _position.x*Game_Settings.UI_Scalar_Unit/_scale;
        _temp.y = Game_Settings.Size.y + _position.y*Game_Settings.UI_Scalar_Unit/_scale;

    }
    else
    {

        _temp.x = _position.x*Game_Settings.UI_Scalar_Unit/_scale;
        _temp.y = _position.y*Game_Settings.UI_Scalar_Unit/_scale;

    }

    return _temp;

}

//Loads the options from the corresponding file
void Load_Options(string _file_Name)
{

    FILE *Options_File;
    Options_File = fopen(_file_Name.c_str(),"r");

    char _temp[100];

    if(Options_File == NULL)
    {
        perror("Error!");
        exit(1);
    }

    while(fgets (_temp, 100, Options_File) != NULL)
    {

        if(String_Contains(_temp,"Window_Width"))
        {

            Game_Settings.Size.x = stoi(Return_Value(_temp,"Window_Width"));

        }
        else if(String_Contains(_temp,"Window_Height"))
        {

            Game_Settings.Size.y = stoi(Return_Value(_temp,"Window_Height"));

        }
        else if(String_Contains(_temp,"RatioX"))
        {

            Game_Settings.Ratio.x = stoi(Return_Value(_temp,"RatioX"));

        }
        else if(String_Contains(_temp,"RatioY"))
        {

            Game_Settings.Ratio.y = stoi(Return_Value(_temp,"RatioY"));

        }
        else if(String_Contains(_temp,"Fullscreen"))
        {

            Game_Settings.Fullscreen = stoi(Return_Value(_temp,"Fullscreen"));

        }
        else if(String_Contains(_temp,"English"))
        {

            Game_Settings.English = stoi(Return_Value(_temp,"English"));

        }
        else if(String_Contains(_temp,"Piece_Color"))
        {

            Game_Settings.Piece_Color = Return_Value(_temp,"Piece_Color");

        }

    }

    Game_Settings.UI_Scalar_Unit = Game_Settings.Size.y/Game_Settings.Ratio.y;

    Temp_Game_Settings = Game_Settings;

    fclose(Options_File);

}

//This function loads the UI elements from a given file
void Load_UI(string _file_Name)
{

    FILE *UI_File;
    UI_File = fopen(_file_Name.c_str(),"r");

    char _temp[100];

    UI_Element _temp_UI_Element;

    if(UI_File == NULL)
    {
        perror("Error!");
        exit(1);
    }

    while(fgets (_temp, 100, UI_File) != NULL)
    {

        if(String_Contains(_temp,"Texture"))
        {

            _temp_UI_Element.UI_Element_Texture.Texture_Adr = Return_Value(_temp,"Texture");
            _temp_UI_Element.UI_Element_Texture.Texture = slLoadTexture(_temp_UI_Element.UI_Element_Texture.Texture_Adr.c_str());

        }
        else if(String_Contains(_temp,"Alignment"))
        {

            _temp_UI_Element.Center_Alignment = Return_Value(_temp,"Alignment");

        }
        else if(String_Contains(_temp,"PositionX"))
        {

            _temp_UI_Element.Position.x = stoi(Return_Value(_temp,"PositionX"));

        }
        else if(String_Contains(_temp,"PositionY"))
        {

            _temp_UI_Element.Position.y = stoi(Return_Value(_temp,"PositionY"));

        }
        else if(String_Contains(_temp,"DimensionX"))
        {

            _temp_UI_Element.Dimensions.x = stoi(Return_Value(_temp,"DimensionX"));

        }
        else if(String_Contains(_temp,"DimensionY"))
        {

            _temp_UI_Element.Dimensions.y = stoi(Return_Value(_temp,"DimensionY"));

        }
        else if(String_Contains(_temp,"Function_Tag"))
        {

            _temp_UI_Element.Function_Tag = Return_Value(_temp,"Function_Tag");

        }
        else if(String_Contains(_temp,"Window_UI_Tag"))
        {

            _temp_UI_Element.Window_UI_Tag = Return_Value(_temp,"Window_UI_Tag");

        }
        else if(String_Contains(_temp,"Displayed_String_EN"))
        {

            _temp_UI_Element.Displayed_String_EN = Return_Value(_temp,"Displayed_String_EN");

        }
        else if(String_Contains(_temp,"Displayed_String_RO"))
        {

            _temp_UI_Element.Displayed_String_RO = Return_Value(_temp,"Displayed_String_RO");

        }
        else if(String_Contains(_temp,"Scale_Modifier"))
        {

            _temp_UI_Element.Scale_Modifier = stoi(Return_Value(_temp,"Scale_Modifier"));

        }
        else if(String_Contains(_temp,"End"))
        {

            _temp_UI_Element.Actual_Dimensions.x = (_temp_UI_Element.Dimensions.x * Game_Settings.UI_Scalar_Unit)/_temp_UI_Element.Scale_Modifier;
            _temp_UI_Element.Actual_Dimensions.y = (_temp_UI_Element.Dimensions.y * Game_Settings.UI_Scalar_Unit)/_temp_UI_Element.Scale_Modifier;
            _temp_UI_Element.Center = UI_Element_Alignment(_temp_UI_Element.Position, _temp_UI_Element.Scale_Modifier,_temp_UI_Element.Center_Alignment);
            Window_UI_Vector.push_back(_temp_UI_Element);
            _temp_UI_Element.Displayed_String_EN = "";
            _temp_UI_Element.Displayed_String_RO = "";

        }



    }

    fclose(UI_File);

}

//This file calculates the area where you can interact the board and other things
void Calculate_Board_HitBox()
{

    Window_UI_Vector[Game_Board.UI_Vector_Position].Center = UI_Element_Alignment(Window_UI_Vector[Game_Board.UI_Vector_Position].Position, 1,    Window_UI_Vector[Game_Board.UI_Vector_Position].Center_Alignment);

    Game_Board.Empty_Tile.x = Game_Board.Size.x/2;
    Game_Board.Empty_Tile.y = Game_Board.Size.y/2;

    Game_Board.Tile_Size = ((Game_Board.Size.y >= Game_Board.Size.x) ? (Window_UI_Vector[Game_Board.UI_Vector_Position].Dimensions.y*Game_Settings.UI_Scalar_Unit)/Game_Board.Size.y : (Window_UI_Vector[Game_Board.UI_Vector_Position].Dimensions.x*Game_Settings.UI_Scalar_Unit)/Game_Board.Size.x);

    Window_UI_Vector[Game_Board.UI_Vector_Position].Actual_Dimensions.x = Game_Board.Tile_Size * Game_Board.Size.x;
    Window_UI_Vector[Game_Board.UI_Vector_Position].Actual_Dimensions.y = Game_Board.Tile_Size * Game_Board.Size.y;

}

//This file loads the board settings from the given file
void Load_Board(string _file_Name)
{

    char _temp[100];

    FILE *Board_File;
    Board_File = fopen(_file_Name.c_str(),"r");

    UI_Element _temp_UI_Element;

    if(Board_File == NULL)
    {
        perror("Error!");
        exit(1);
    }

    while(fgets (_temp, 100, Board_File) != NULL)
    {

        if(String_Contains(_temp,"SizeX"))
        {

            Game_Board.Size.x = stoi(Return_Value(_temp,"SizeX"));

        }
        else if(String_Contains(_temp,"SizeY"))
        {

            Game_Board.Size.y = stoi(Return_Value(_temp,"SizeY"));

        }
        else if(String_Contains(_temp,"Margin"))
        {

            Game_Board.Tile_Margine = stoi(Return_Value(_temp,"Margin"));

        }
        else if(String_Contains(_temp,"StepSizeX"))
        {

            Game_Board.Board_Pattern.Step_Size.x = stoi(Return_Value(_temp,"StepSizeX"));

        }
        else if(String_Contains(_temp,"StepSizeY"))
        {

            Game_Board.Board_Pattern.Step_Size.y = stoi(Return_Value(_temp,"StepSizeY"));

        }
        else if(String_Contains(_temp,"StepNo"))
        {

            Game_Board.Board_Pattern.Step_Start_Number = stoi(Return_Value(_temp,"StepNo"));

        }
        else if(String_Contains(_temp,"StepDec"))
        {

            Game_Board.Board_Pattern.Step_Decline = stoi(Return_Value(_temp,"StepDec"));

        }
        else if(String_Contains(_temp,"Alignment"))
        {

            _temp_UI_Element.Center_Alignment = Return_Value(_temp,"Alignment");

        }
        else if(String_Contains(_temp,"DimensionX"))
        {

            _temp_UI_Element.Dimensions.x = stoi(Return_Value(_temp,"DimensionX"));

        }
        else if(String_Contains(_temp,"DimensionY"))
        {

            _temp_UI_Element.Dimensions.y = stoi(Return_Value(_temp,"DimensionY"));

        }
        else if(String_Contains(_temp,"PosX"))
        {

            _temp_UI_Element.Position.x = stoi(Return_Value(_temp,"PosX"));

        }
        else if(String_Contains(_temp,"PosY"))
        {

            _temp_UI_Element.Position.y = stoi(Return_Value(_temp,"PosY"));

        }
        else if(String_Contains(_temp,"Function_Tag"))
        {

            _temp_UI_Element.Function_Tag = Return_Value(_temp,"Function_Tag");

        }
        else if(String_Contains(_temp,"Window_UI_Tag"))
        {

            _temp_UI_Element.Window_UI_Tag = Return_Value(_temp,"Window_UI_Tag");

        }
        else if(String_Contains(_temp,"Score"))
        {

            for(int i = 0; i < 5; i++)
            {

                if(String_Contains(_temp, (to_string(i) + "s")))
                {

                    Game_Board.Score_Board[i].Score = stoi(Return_Value(Return_Value(_temp,(to_string(i) + "s")), "Score"));

                    break;

                }

            }

        }
        else if(String_Contains(_temp,"Initial"))
        {

            for(int i = 0; i < 5; i++)
            {

                if(String_Contains(_temp, (to_string(i) + "i")))
                {

                    Game_Board.Score_Board[i].Initial = Return_Value(Return_Value(_temp,(to_string(i) + "i")), "Initial");

                    break;

                }

            }

        }
        else if(String_Contains(_temp,"Time"))
        {

            for(int i = 0; i < 5; i++)
            {

                if(String_Contains(_temp, (to_string(i) + "t")))
                {

                    Game_Board.Score_Board[i].Time = stoi(Return_Value(Return_Value(_temp,(to_string(i) + "t")), "Time"));

                    break;

                }

            }

        }

    }

    while(!(Game_Board.Score_Board[0].Score >= Game_Board.Score_Board[1].Score &&
            Game_Board.Score_Board[1].Score >= Game_Board.Score_Board[2].Score &&
            Game_Board.Score_Board[2].Score >= Game_Board.Score_Board[3].Score&&
            Game_Board.Score_Board[3].Score >= Game_Board.Score_Board[4].Score))
    {

        for(int i = 1; i < 5; i++)
        {

            if(Game_Board.Score_Board[i].Score > Game_Board.Score_Board[i - 1].Score)
            {

                Game_Board.Current_Score_Board[0] = Game_Board.Score_Board[i];
                Game_Board.Score_Board[i] = Game_Board.Score_Board[i - 1];
                Game_Board.Score_Board[i - 1] = Game_Board.Current_Score_Board[0];

            }

        }

    }

    for(int i = 0; i < 5; i++)
    {

        Game_Board.Current_Score_Board[i] = Game_Board.Score_Board[i];

    }

    _temp_UI_Element.Center = UI_Element_Alignment(_temp_UI_Element.Position, 1,_temp_UI_Element.Center_Alignment);

    Game_Board.Time = 0;

    Game_Board.Current_Score_Rank = 5;

    Game_Board.Current_Score_Board[5].Initial = "";
    Game_Board.Current_Score_Board[5].Score = 0;
    Game_Board.Current_Score_Board[5].Time = 0;
    Game_Board.Time = 0;

    if(Game_Board.UI_Vector_Position == NULL)
    {

        Window_UI_Vector.push_back(_temp_UI_Element);

        Game_Board.UI_Vector_Position = Window_UI_Vector.size() - 1;

    }
    else
    {

        Window_UI_Vector[Game_Board.UI_Vector_Position] = _temp_UI_Element;

    }

    Calculate_Board_HitBox();

    Create_Matrix();

    fclose(Board_File);

}

//This function assigns the value of the starting states of each tile on the board into a matrix
//-1 - don't render
//0 - empty tile
//1 - tile with piece
//2 - selected tile with piece
//3 - possible move
void Create_Matrix()
{

    No_Of_Pieces = 0;

    int _current_Step_Number = Game_Board.Board_Pattern.Step_Start_Number;

    for(int i = 0; i < Game_Board.Size.x*Game_Board.Size.y; i++)
    {

        if(Game_Board.Size.x/2 - Game_Board.Board_Pattern.Step_Size.x * _current_Step_Number >= abs(i%Game_Board.Size.x - Game_Board.Size.x/2))
        {


            *(Game_Board.Matrix + i) = 1;

            No_Of_Pieces++;

        }
        else
        {

            *(Game_Board.Matrix + i) = -1;

        }

        if(_current_Step_Number > 0 &&
                i%Game_Board.Size.x == Game_Board.Size.x - 1 &&
                (i/Game_Board.Size.x)%Game_Board.Board_Pattern.Step_Size.y == Game_Board.Board_Pattern.Step_Size.y - 1 &&
                (i/Game_Board.Size.x) < Game_Board.Size.y - Game_Board.Board_Pattern.Step_Size.y*(Game_Board.Board_Pattern.Step_Start_Number/Game_Board.Board_Pattern.Step_Decline) - 1)
        {

            _current_Step_Number = _current_Step_Number - Game_Board.Board_Pattern.Step_Decline;

        }
        else if(_current_Step_Number < Game_Board.Board_Pattern.Step_Start_Number  &&
                i%Game_Board.Size.x == Game_Board.Size.x - 1 &&
                /*((i/Game_Board.Size.x) + 1)%Game_Board.Board_Pattern.Step_Size.y == Game_Board.Board_Pattern.Step_Size.y - 1 &&*/
                (i/Game_Board.Size.x) >= Game_Board.Size.y - Game_Board.Board_Pattern.Step_Size.y*(Game_Board.Board_Pattern.Step_Start_Number/Game_Board.Board_Pattern.Step_Decline) - 1)
        {

            _current_Step_Number = _current_Step_Number +
                                   Game_Board.Board_Pattern.Step_Decline;

        }

    }


    No_Of_Pieces--;
    *(Game_Board.Matrix + (Game_Board.Empty_Tile.y * Game_Board.Size.x + Game_Board.Empty_Tile.x)) = 0;


}

//This bool returns the state of the board
bool Game_Over()
{
    for(int i=0; i<Game_Board.Size.x*Game_Board.Size.y; i++)
    {
        if(*(Game_Board.Matrix + i)==1||*(Game_Board.Matrix + i)==2)
        {

            if(i+2*Game_Board.Size.x<=Game_Board.Size.x*Game_Board.Size.y &&(*(Game_Board.Matrix + i+2*Game_Board.Size.x)==0||*(Game_Board.Matrix + i+2*Game_Board.Size.x)==3)&&(*(Game_Board.Matrix + i+Game_Board.Size.x)==1||*(Game_Board.Matrix + i+Game_Board.Size.x)==2))
            {
                return false;
            }

            if(i-2*Game_Board.Size.x >=0 &&(*(Game_Board.Matrix + i-2*Game_Board.Size.x)==0||*(Game_Board.Matrix + i-2*Game_Board.Size.x)==3)&&(*(Game_Board.Matrix + i-Game_Board.Size.x)==1||*(Game_Board.Matrix + i-Game_Board.Size.x)==2))
            {
                return false;
            }

            if((i+2)/Game_Board.Size.x ==i/Game_Board.Size.x &&(*(Game_Board.Matrix + i+2)==0||*(Game_Board.Matrix + i+2)==3)&&(*(Game_Board.Matrix + i+1)==1||*(Game_Board.Matrix + i+1)==2))
            {
                return false;
            }

            if((i-2)/Game_Board.Size.x ==i/Game_Board.Size.x &&(*(Game_Board.Matrix + i-2)==0||*(Game_Board.Matrix + i-2)==3)&&(*(Game_Board.Matrix + i-1)==1||*(Game_Board.Matrix + i-1)==2))
            {
                return false;
            }
        }
    }

    return true;
}

//This function draws the board
void Draw_Board()
{

    for(int i = 0; i < Game_Board.Size.x*Game_Board.Size.y; i++)
    {

        if(*(Game_Board.Matrix + i) != -1)
        {

            if(*(Game_Board.Matrix + i) == 3)
            {

                slSetForeColor(1,1,0,1);

            }
            else if(*(Game_Board.Matrix + i) == 2)
            {

                slSetForeColor(0.1,0.2,0.6,1);

            }
            else
            {

                slSetForeColor(1,1,1,1);

            }
            if((End_Game && Current_Window_UI_Tag == "Game") || (End_Show_Game && Current_Window_UI_Tag == "Main_Menu"))
            {
                if(Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank].Score != No_Of_Pieces-1)
                {
                    slSetForeColor(1,0,0,1);
                }

                else
                {
                    slSetForeColor(0,1,0,1);
                }
            }

            slRectangleFill((Window_UI_Vector[Game_Board.UI_Vector_Position].Center.x - (Window_UI_Vector[Game_Board.UI_Vector_Position].Actual_Dimensions.x)/2 + Game_Board.Tile_Size/2 + Game_Board.Tile_Size*(i % Game_Board.Size.x)),
                            (Window_UI_Vector[Game_Board.UI_Vector_Position].Center.y - (Window_UI_Vector[Game_Board.UI_Vector_Position].Actual_Dimensions.y)/2 + Game_Board.Tile_Size/2 + Game_Board.Tile_Size*(i / Game_Board.Size.x)),
                            Game_Board.Tile_Size - Game_Board.Tile_Margine, Game_Board.Tile_Size - Game_Board.Tile_Margine
                           );


        }
        if(*(Game_Board.Matrix + i) == 1 || *(Game_Board.Matrix + i) == 2)
        {

            if(Game_Settings.Piece_Color == "Green")
            {

                slSetForeColor(0,0.2,0.1,1);

            }
            else if(Game_Settings.Piece_Color == "Red")
            {

                slSetForeColor(0.25,0,0,1);

            }
            else if(Game_Settings.Piece_Color == "Blue")
            {

                slSetForeColor(0,0.1,0.25,1);

            }
            else if(Game_Settings.Piece_Color == "Yellow")
            {

                slSetForeColor(0.31,0.3,0.08,1);

            }

            slCircleFill((Window_UI_Vector[Game_Board.UI_Vector_Position].Center.x - (Window_UI_Vector[Game_Board.UI_Vector_Position].Actual_Dimensions.x)/2 + Game_Board.Tile_Size/2.1 + Game_Board.Tile_Size*(i % Game_Board.Size.x)),
                         (Window_UI_Vector[Game_Board.UI_Vector_Position].Center.y - (Window_UI_Vector[Game_Board.UI_Vector_Position].Actual_Dimensions.y)/2 + Game_Board.Tile_Size/2.3 + Game_Board.Tile_Size*(i / Game_Board.Size.x)),
                         Game_Board.Tile_Size/3, 20);

            if(Game_Settings.Piece_Color == "Green")
            {

                slSetForeColor(0.1,0.4,0.2,1);

            }
            else if(Game_Settings.Piece_Color == "Red")
            {

                slSetForeColor(0.5,0.1,0,1);

            }
            else if(Game_Settings.Piece_Color == "Blue")
            {

                slSetForeColor(0,0.2,0.5,1);

            }
            else if(Game_Settings.Piece_Color == "Yellow")
            {

                slSetForeColor(0.62,0.6,0.16,1);

            }

            slCircleFill((Window_UI_Vector[Game_Board.UI_Vector_Position].Center.x - (Window_UI_Vector[Game_Board.UI_Vector_Position].Actual_Dimensions.x)/2 + Game_Board.Tile_Size/2 + Game_Board.Tile_Size*(i % Game_Board.Size.x)),
                         (Window_UI_Vector[Game_Board.UI_Vector_Position].Center.y - (Window_UI_Vector[Game_Board.UI_Vector_Position].Actual_Dimensions.y)/2 + Game_Board.Tile_Size/2 + Game_Board.Tile_Size*(i / Game_Board.Size.x)),
                         Game_Board.Tile_Size/3, 20
                        );

        }

    }



}

double Last_Move_Time;
int Current_Move=1;
//This void draws the UI on the screen
void Draw_UI()
{

    Vector2 _temp_Position_Holder;

    if(Current_Window_UI_Tag == "Game" && !End_Game)
    {

        Game_Board.Time += slGetDeltaTime();
        Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank].Time = (int)Game_Board.Time;

    }


    slSetBackColor(0,0,0);

    if(Current_Window_UI_Tag == "Game_Select_Menu")
    {

        _temp_Position_Holder.x = 3;
        _temp_Position_Holder.y = 6;

        _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder,2,"LC");

        slSetFontSize(Game_Settings.UI_Scalar_Unit/4);
        slSetForeColor(1,1,1,1);
        if(Game_Settings.English)
        {

            slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"English");

        }
        else
        {

            slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Englezeasca");

        }

        _temp_Position_Holder.x = 3;
        _temp_Position_Holder.y = 2;

        _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder,2,"LC");

        slSetFontSize(Game_Settings.UI_Scalar_Unit/4);
        slSetForeColor(1,1,1,1);

        if(Game_Settings.English)
        {

            slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Continental");

        }
        else
        {

            slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Continentala");

        }

        _temp_Position_Holder.x = 3;
        _temp_Position_Holder.y = -2;

        _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder,2,"LC");

        slSetFontSize(Game_Settings.UI_Scalar_Unit/4);
        slSetForeColor(1,1,1,1);

        slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Extra");

        if(Current_Board_Index != 0)
        {

            _temp_Position_Holder.x = 9 + ((Current_Board_Index - 1)%4)*5;
            _temp_Position_Holder.y = 6 - ((Current_Board_Index - 1)/4)*4;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 2,"LC");

            slSetForeColor(1,1,0,1);
            slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, (9*Game_Settings.UI_Scalar_Unit)/4, (9*Game_Settings.UI_Scalar_Unit)/8);

        }
        else if (Current_Board_Index == 0)
        {

            _temp_Position_Holder.x = 9;
            _temp_Position_Holder.y = -2;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 2,"LC");

            slSetForeColor(1,1,0,1);
            slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, (9*Game_Settings.UI_Scalar_Unit)/4, (9*Game_Settings.UI_Scalar_Unit)/8);

        }

    }
    else if(Current_Window_UI_Tag == "Main_Option_Menu")
    {

        _temp_Position_Holder.x = 5;
        _temp_Position_Holder.y = 6;

        _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder,2,"LC");

        slSetFontSize(Game_Settings.UI_Scalar_Unit/2.5);
        slSetForeColor(1,1,1,1);

        if(Game_Settings.English)
        {

            slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Window Mode");

        }
        else
        {

            slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Mod Fereastra");

        }

        _temp_Position_Holder.x = 0;
        _temp_Position_Holder.y = 2;

        _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder,2,"C");

        slSetFontSize(Game_Settings.UI_Scalar_Unit/2.5);
        slSetForeColor(1,1,1,1);

        if(Game_Settings.English)
        {

            slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Resolution");

        }
        else
        {

            slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Rezolutie");

        }

        if(Temp_Game_Settings.Fullscreen)
        {

            _temp_Position_Holder.x = 16;
            _temp_Position_Holder.y = 6;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 2,"LC");

            slSetForeColor(1,1,0,1);
            slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, (9*Game_Settings.UI_Scalar_Unit)/2.1, (9*Game_Settings.UI_Scalar_Unit)/8);

        }
        else
        {

            _temp_Position_Holder.x = 25;
            _temp_Position_Holder.y = 6;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 2,"LC");

            slSetForeColor(1,1,0,1);
            slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, (9*Game_Settings.UI_Scalar_Unit)/2.1, (9*Game_Settings.UI_Scalar_Unit)/8);

        }

        if(Temp_Game_Settings.Size.y == 1080)
        {

            _temp_Position_Holder.x = -7;
            _temp_Position_Holder.y = 0;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 2,"RC");

            slSetForeColor(1,1,0,1);
            slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, (9*Game_Settings.UI_Scalar_Unit)/2.1, (9*Game_Settings.UI_Scalar_Unit)/8);

        }
        else if (Temp_Game_Settings.Size.y == 720)
        {

            _temp_Position_Holder.x = 0;
            _temp_Position_Holder.y = 0;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 2,"C");

            slSetForeColor(1,1,0,1);
            slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, (9*Game_Settings.UI_Scalar_Unit)/2.1, (9*Game_Settings.UI_Scalar_Unit)/8);

        }
        else if (Temp_Game_Settings.Size.y == 480)
        {

            _temp_Position_Holder.x = 7;
            _temp_Position_Holder.y = 0;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 2,"LC");

            slSetForeColor(1,1,0,1);
            slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, (9*Game_Settings.UI_Scalar_Unit)/2.1, (9*Game_Settings.UI_Scalar_Unit)/8);

        }


    }

    for(int i = 0; i < Window_UI_Vector.size(); i++)
    {

        if(Window_UI_Vector[i].Function_Tag != "Game_Board" && Window_UI_Vector[i].Function_Tag != "Game_Show_Board" && Window_UI_Vector[i].Window_UI_Tag == Current_Window_UI_Tag)
        {

            slSetForeColor(1,1,1,1);
            slSprite(Window_UI_Vector[i].UI_Element_Texture.Texture, Window_UI_Vector[i].Center.x, Window_UI_Vector[i].Center.y, Window_UI_Vector[i].Actual_Dimensions.x, Window_UI_Vector[i].Actual_Dimensions.y);

            if(Window_UI_Vector[i].Displayed_String_EN != "" && Game_Settings.English)
            {

                slSetFontSize(Game_Settings.UI_Scalar_Unit/(2 * Window_UI_Vector[i].Scale_Modifier));
                slText(Window_UI_Vector[i].Center.x, Window_UI_Vector[i].Center.y - Game_Settings.UI_Scalar_Unit/(3 *  Window_UI_Vector[i].Scale_Modifier), Window_UI_Vector[i].Displayed_String_EN.c_str());

            }
            else if(Window_UI_Vector[i].Displayed_String_RO != "" && !Game_Settings.English)
            {

                slSetFontSize(Game_Settings.UI_Scalar_Unit/(2 * Window_UI_Vector[i].Scale_Modifier));
                slText(Window_UI_Vector[i].Center.x, Window_UI_Vector[i].Center.y - Game_Settings.UI_Scalar_Unit/(3 *  Window_UI_Vector[i].Scale_Modifier), Window_UI_Vector[i].Displayed_String_RO.c_str());

            }

            if(((abs(slGetMouseX() - Window_UI_Vector[i].Center.x) <= Window_UI_Vector[i].Actual_Dimensions.x/2) && abs(slGetMouseY() - Window_UI_Vector[i].Center.y) <= Window_UI_Vector[i].Actual_Dimensions.y/2) && Window_UI_Vector[i].Window_UI_Tag == Current_Window_UI_Tag)
            {

                slSetForeColor(1,1,1,0.2);
                slRectangleFill(Window_UI_Vector[i].Center.x, Window_UI_Vector[i].Center.y, Window_UI_Vector[i].Actual_Dimensions.x, Window_UI_Vector[i].Actual_Dimensions.y);

            }

        }

    }

    if(Current_Window_UI_Tag == "Game")
    {

        Draw_Board();

        string _temp_String;

        if(Game_Settings.English)
        {

            _temp_String = "Score Board";

        }
        else
        {

            _temp_String = "Tabla de scor";

        }

        _temp_Position_Holder.x = -6;
        _temp_Position_Holder.y = -2;

        _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 2,"TR");

        slSetFontSize(Game_Settings.UI_Scalar_Unit/2);
        slSetForeColor(1,1,1,1);
        slText(_temp_Position_Holder.x,_temp_Position_Holder.y,_temp_String.c_str());

        for(int i = 0; i < 6; i++)
        {

            _temp_String = Game_Board.Current_Score_Board[i].Initial + ": " + to_string(Game_Board.Current_Score_Board[i].Score) + "/" + to_string(Game_Board.Current_Score_Board[i].Time) + "s";

            _temp_Position_Holder.x = -5;
            _temp_Position_Holder.y = -2 - (i + 1) * 2;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 2,"TR");

            slSetFontSize(Game_Settings.UI_Scalar_Unit/2);
            slSetForeColor(1,1,1,1);
            slText(_temp_Position_Holder.x,_temp_Position_Holder.y,_temp_String.c_str());

        }

        if(!Enable_Undo || End_Game)
        {

            _temp_Position_Holder.x = -1;
            _temp_Position_Holder.y = 1;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 2,"DR");

            slSetForeColor(0,0,0,0.25);
            slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, Game_Settings.UI_Scalar_Unit/2, Game_Settings.UI_Scalar_Unit/2);

        }

        if(End_Game)
        {

            _temp_Position_Holder.x = 0;
            _temp_Position_Holder.y = 0;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 1,"C");

            slSetFontSize(Game_Settings.UI_Scalar_Unit);
            slSetForeColor(1,1,1,1);

            if(Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank].Score == No_Of_Pieces-1)
            {

                if(Game_Settings.English)
                {

                    slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"You Win");

                }
                else
                {

                    slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Ai castigat");

                }

            }
            else
            {

                if(Game_Settings.English)
                {

                    slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Game Over");

                }
                else
                {

                    slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Joc terminat");

                }

            }

            _temp_Position_Holder.x = -3;
            _temp_Position_Holder.y = 1;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 2,"DR");

            slSetForeColor(0,0,0,0.25);
            slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, Game_Settings.UI_Scalar_Unit/2, Game_Settings.UI_Scalar_Unit/2);

            if(Take_Input_From_Keyboard)
            {

                _temp_Position_Holder.x = 0;
                _temp_Position_Holder.y = -3;

                _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder,2,"C");

                slSetForeColor(1,1,1,1);
                slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, 4*Game_Settings.UI_Scalar_Unit, 1.5*Game_Settings.UI_Scalar_Unit);
                slSetForeColor(0,0,0,1);
                slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, 3.5*Game_Settings.UI_Scalar_Unit, 1*Game_Settings.UI_Scalar_Unit);

                _temp_Position_Holder.x = 0;
                _temp_Position_Holder.y = -5;

                _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder,3,"C");

                slSetFontSize(Game_Settings.UI_Scalar_Unit/2.5);
                slSetForeColor(1,1,1,1);

                slText(_temp_Position_Holder.x,_temp_Position_Holder.y,Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank].Initial.c_str());

            }

        }

    }
    else if(Current_Window_UI_Tag == "Main_Menu")
    {

        if(slGetTime()>=Last_Move_Time+0.75 && Current_Move<=Best)
        {
        Deselect_Piece();
            *(Game_Board.Matrix + Solution[Current_Move].Start)=0;
            *(Game_Board.Matrix + Solution[Current_Move].Middle)=0;
            *(Game_Board.Matrix + Solution[Current_Move].End)=1;
            Last_Move_Time=slGetTime();
            Current_Move++;
            if(Current_Move<=Best)
            Selected_Tile=Solution[Current_Move].Start;
            Select_Piece();
        }
        else
        {
            if(Current_Move>Best && slGetTime()>=Last_Move_Time+0.75)
            {
                Load_Board(Board_Address_Array[0]);
                Current_Move=1;
            }
        }

        if(End_Game)
        {


            _temp_Position_Holder.x = 5;
            _temp_Position_Holder.y = 3;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder,2,"LC");

            slSetForeColor(0,0,0,0.25);
            slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, 4*Game_Settings.UI_Scalar_Unit, Game_Settings.UI_Scalar_Unit);

        }

        if(Game_Settings.English)
        {


            _temp_Position_Holder.x = 9;
            _temp_Position_Holder.y = 5;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 2,"LC");

            slSetFontSize(Game_Settings.UI_Scalar_Unit);
            slSetForeColor(1,1,1,1);

            slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Peg Solitare");

        }
        else
        {


            _temp_Position_Holder.x = 7;
            _temp_Position_Holder.y = 5;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder, 2,"LC");

            slSetFontSize(Game_Settings.UI_Scalar_Unit*1.1);
            slSetForeColor(1,1,1,1);

            slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Solitarul");

        }

        Draw_Board();

    }
    else if(Current_Window_UI_Tag == "Main_Option_Menu")
    {

        if(Show_Close_Window_Dialogue)
        {

            _temp_Position_Holder.x = 0;
            _temp_Position_Holder.y = 0;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder,2,"C");

            slSetForeColor(1,1,1,1);
            slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, 15*Game_Settings.UI_Scalar_Unit, 2*Game_Settings.UI_Scalar_Unit);
            slSetForeColor(0,0,0,1);
            slRectangleFill(_temp_Position_Holder.x, _temp_Position_Holder.y, 14*Game_Settings.UI_Scalar_Unit, 1.5*Game_Settings.UI_Scalar_Unit);

            _temp_Position_Holder.x = 0;
            _temp_Position_Holder.y = -1;

            _temp_Position_Holder = UI_Element_Alignment(_temp_Position_Holder,4,"C");

            slSetFontSize(Game_Settings.UI_Scalar_Unit/2.5);
            slSetForeColor(1,1,1,1);

            if(Game_Settings.English)
            {

                slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Please restart the game for changes to take hold");

            }
            else
            {

                slText(_temp_Position_Holder.x,_temp_Position_Holder.y,"Va rog restartati jocul");

            }

        }

    }

    slRender();

}

//This function returns the tag of the closes UI_Element
string Find_UI_Element_Tag()
{

    for(int i = 0; i < Window_UI_Vector.size(); i++)
    {

        if(((abs(slGetMouseX() - Window_UI_Vector[i].Center.x) <= Window_UI_Vector[i].Actual_Dimensions.x/2) && abs(slGetMouseY() - Window_UI_Vector[i].Center.y) <= Window_UI_Vector[i].Actual_Dimensions.y/2) && Window_UI_Vector[i].Window_UI_Tag == Current_Window_UI_Tag)
        {

            return Window_UI_Vector[i].Function_Tag;

        }

    }

    return "";

}

//This function executes the diferent comands associated to each interactable UI element
void UI_Decoder(string _tag)
{

    if(_tag == "In_Game_Pause_Button")
    {
        Current_Move=1;
        Deselect_Piece();
        Current_Window_UI_Tag = "Main_Menu";
        Temp_Game_Board = Game_Board;
        Temp_UI_Board_Element = Window_UI_Vector[Game_Board.UI_Vector_Position];
        Game_Board.Matrix = Menu_Board_Int_Array;
        Load_Board(Board_Address_Array[0]);
        Take_Input_From_Keyboard = false;
        Has_Written_To_File = false;

    }
    else if(_tag == "In_Game_Undo_Last_Move_Button" && !End_Game)
    {

        Undo();

    }
    else if(_tag == "Main_Menu_New_Game_Button")
    {

        Current_Window_UI_Tag = "Game_Select_Menu";

    }
    else if(_tag == "Main_Menu_Continue_Button" && !End_Game)
    {

        Game_Board = Temp_Game_Board;
        Game_Board.Matrix = Main_Board_Int_Array;
        Window_UI_Vector[Game_Board.UI_Vector_Position] = Temp_UI_Board_Element;
        Current_Window_UI_Tag = "Game";
        Has_Written_To_File = false;
        Take_Input_From_Keyboard = true;

    }
    else if(_tag == "Main_Menu_Quit_Button")
    {

        Window_Closed = true;

    }
    else if(_tag == "Main_Menu_Options_Button")
    {

        Current_Window_UI_Tag = "Main_Option_Menu";

    }
    else if(_tag == "In_Game_Restart_Button")
    {

        Create_Matrix();

        End_Game = false;

        Game_Board.Current_Score_Rank = 5;

        Game_Board.Current_Score_Board[0] = Game_Board.Score_Board[0];
        Game_Board.Current_Score_Board[1] = Game_Board.Score_Board[1];
        Game_Board.Current_Score_Board[2] = Game_Board.Score_Board[2];
        Game_Board.Current_Score_Board[3] = Game_Board.Score_Board[3];
        Game_Board.Current_Score_Board[4] = Game_Board.Score_Board[4];
        Game_Board.Current_Score_Board[5].Initial = "";
        Game_Board.Current_Score_Board[5].Score = 0;
        Game_Board.Current_Score_Board[5].Time = 0;
        Game_Board.Time = 0;
        Take_Input_From_Keyboard = true;
        Has_Written_To_File = false;
        Enable_Undo = false;


    }
    else if(_tag == "Main_Option_Menu_Back_Button")
    {

        Temp_Game_Settings = Game_Settings;

        Current_Window_UI_Tag = "Main_Menu";

    }
    else if(_tag == "Main_Option_Menu_Apply_Button")
    {

        if(Temp_Game_Settings.Size.y != Game_Settings.Size.y || Temp_Game_Settings.Fullscreen != Game_Settings.Fullscreen)
        {

            Show_Close_Window_Dialogue = true;

            Draw_UI();

            Wait(1);

            Show_Close_Window_Dialogue = false;

        }

        if(Temp_Game_Settings.Fullscreen != Game_Settings.Fullscreen)
        {

            Write_Value("files/Options_File.txt","Fullscreen",to_string(Temp_Game_Settings.Fullscreen));

        }
        if(Temp_Game_Settings.Size.y != Game_Settings.Size.y)
        {

            Write_Value("files/Options_File.txt","Window_Width",to_string(Temp_Game_Settings.Size.x));
            Write_Value("files/Options_File.txt","Window_Height",to_string(Temp_Game_Settings.Size.y));


        }

        Current_Window_UI_Tag = "Main_Menu";

    }
    else if(_tag == "In_Game_Random_Button" && !End_Game)
    {

        Deselect_Piece();
        Random_Solve();
        Draw_UI();
        Wait(0.25);
        Random_Solve();
        Draw_UI();

    }
    else if(_tag == "Game_Select_Menu_Play_Button")
    {

        Game_Board.Matrix = Main_Board_Int_Array;

        Load_Board(Board_Address_Array[Current_Board_Index]);

        End_Game = false;

        Take_Input_From_Keyboard = true;

        Enable_Undo = false;

        Current_Window_UI_Tag = "Game";

    }
    else if(_tag == "Game_Select_Menu_7x7_Cross_Button")
    {

        Current_Board_Index = 1;

    }
    else if(_tag == "Game_Select_Menu_9x9_Cross_Button")
    {

        Current_Board_Index = 2;

    }
    else if(_tag == "Game_Select_Menu_11x11_Cross_Button")
    {

        Current_Board_Index = 3;

    }
    else if(_tag == "Game_Select_Menu_13x13_Cross_Button")
    {

        Current_Board_Index = 4;

    }
    else if(_tag == "Game_Select_Menu_7x7_Corner_Button")
    {

        Current_Board_Index = 5;

    }
    else if(_tag == "Game_Select_Menu_9x9_Corner_Button")
    {

        Current_Board_Index = 6;

    }
    else if(_tag == "Game_Select_Menu_11x11_Corner_Button")
    {

        Current_Board_Index = 7;

    }
    else if(_tag == "Game_Select_Menu_13x13_Corner_Button")
    {

        Current_Board_Index = 8;

    }
    else if(_tag == "Game_Select_Menu_5x9_Extra_Button")
    {

        Current_Board_Index = 9;

    }
    else if(_tag == "Game_Select_Menu_7x7_Extra_Button")
    {

        Current_Board_Index = 10;

    }
    else if(_tag == "Game_Select_Menu_9x9_Extra_Button")
    {

        Current_Board_Index = 11;

    }
    else if(_tag == "Game_Select_Menu_11x11_Extra_Button")
    {

        Current_Board_Index = 12;

    }
    else if(_tag == "Game_Select_Menu_Back_Button")
    {

        Current_Window_UI_Tag = "Main_Menu";

    }
    else if(_tag == "Main_Option_Menu_Fullscreen_Button")
    {

        Temp_Game_Settings.Fullscreen = true;

    }
    else if(_tag == "Main_Option_Menu_Window_Button")
    {

        Temp_Game_Settings.Fullscreen = false;

    }
    else if(_tag == "Main_Option_Menu_480x852_Button")
    {

        Temp_Game_Settings.Size.x = 852;
        Temp_Game_Settings.Size.y = 480;

    }
    else if(_tag == "Main_Option_Menu_720x1280_Button")
    {

        Temp_Game_Settings.Size.x = 1280;
        Temp_Game_Settings.Size.y = 720;

    }
    else if(_tag == "Main_Option_Menu_1080x1920_Button")
    {

        Temp_Game_Settings.Size.x = 1920;
        Temp_Game_Settings.Size.y = 1080;

    }
    else if(_tag == "Main_Option_Menu_Reset_Button")
    {

        for(int i = 1; i < 13; i++)
        {

            for(int j = 0; j < 5; j++)
            {

                Write_Value(Board_Address_Array[i],(to_string(j) + "s"),"<Score>0</Score>");
                Write_Value(Board_Address_Array[i],(to_string(j) + "i"),"<Initial></Initial>");
                Write_Value(Board_Address_Array[i],(to_string(j) + "t"),"<Time>0</Time>");

            }

        }

    }
    else if(_tag == "Main_Option_Menu_Language_Button")
    {

        Game_Settings.English = !Game_Settings.English;

        Write_Value("files/Options_File.txt","English",to_string(Game_Settings.English));

    }
    else if(_tag == "Piece_Color_Green_Button")
    {

        Game_Settings.Piece_Color = "Green";

        Write_Value("files/Options_File.txt","Piece_Color","Green");

    }
    else if(_tag == "Piece_Color_Red_Button")
    {

        Game_Settings.Piece_Color = "Red";

        Write_Value("files/Options_File.txt","Piece_Color","Red");

    }
    else if(_tag == "Piece_Color_Blue_Button")
    {

        Game_Settings.Piece_Color = "Blue";

        Write_Value("files/Options_File.txt","Piece_Color","Blue");

    }
    else if(_tag == "Piece_Color_Yellow_Button")
    {

        Game_Settings.Piece_Color = "Yellow";

        Write_Value("files/Options_File.txt","Piece_Color","Yellow");

    }

}

//This function stops the program for _delay seconds
void Wait(double _delay)
{
    double _time=slGetTime()+_delay;
    while(slGetTime()<_time);
}

//This function selects a piece and highlights the possible moves
void Select_Piece()
{
    if(*(Game_Board.Matrix + Selected_Tile) == 1)
    {

        *(Game_Board.Matrix + Selected_Tile) = 2;

        if(Selected_Tile+2*Game_Board.Size.x<=Game_Board.Size.x*Game_Board.Size.y &&*(Game_Board.Matrix + Selected_Tile+2*Game_Board.Size.x)==0&&*(Game_Board.Matrix + Selected_Tile+Game_Board.Size.x)==1)
            *(Game_Board.Matrix + Selected_Tile+2*Game_Board.Size.x)=3;

        if(Selected_Tile-2*Game_Board.Size.x >=0 &&*(Game_Board.Matrix + Selected_Tile-2*Game_Board.Size.x)==0&&*(Game_Board.Matrix + Selected_Tile-Game_Board.Size.x)==1)
            *(Game_Board.Matrix + Selected_Tile-2*Game_Board.Size.x)=3;

        if((Selected_Tile+2)/Game_Board.Size.x ==Selected_Tile/Game_Board.Size.x &&*(Game_Board.Matrix + Selected_Tile+2)==0&&*(Game_Board.Matrix + Selected_Tile+1)==1)
            *(Game_Board.Matrix + Selected_Tile+2)=3;

        if((Selected_Tile-2)/Game_Board.Size.x ==Selected_Tile/Game_Board.Size.x &&*(Game_Board.Matrix + Selected_Tile-2)==0&&*(Game_Board.Matrix + Selected_Tile-1)==1)
            *(Game_Board.Matrix + Selected_Tile-2)=3;

    }
}

//This function deselects the selected piece and removes highlights
void Deselect_Piece()
{
    if(*(Game_Board.Matrix + Selected_Tile) == 2)
    {

        *(Game_Board.Matrix + Selected_Tile) = 1;

    }
    if(Selected_Tile+2*Game_Board.Size.x<=Game_Board.Size.x*Game_Board.Size.y &&*(Game_Board.Matrix + Selected_Tile+2*Game_Board.Size.x)==3)
        *(Game_Board.Matrix + Selected_Tile+2*Game_Board.Size.x)=0;
    if(Selected_Tile-2*Game_Board.Size.x >=0 &&*(Game_Board.Matrix + Selected_Tile-2*Game_Board.Size.x)==3)
        *(Game_Board.Matrix + Selected_Tile-2*Game_Board.Size.x)=0;
    if((Selected_Tile+2)/Game_Board.Size.x ==Selected_Tile/Game_Board.Size.x &&*(Game_Board.Matrix + Selected_Tile+2)==3)
        *(Game_Board.Matrix + Selected_Tile+2)=0;
    if((Selected_Tile-2)/Game_Board.Size.x ==Selected_Tile/Game_Board.Size.x &&*(Game_Board.Matrix + Selected_Tile-2)==3)
        *(Game_Board.Matrix + Selected_Tile-2)=0;
}

int Und[3];

//This function undoes the last move
void Undo()
{
    if(Enable_Undo)
    {
        Enable_Undo=false;
        *(Game_Board.Matrix + Und[0])=1;
        *(Game_Board.Matrix + Und[1])=1;
        *(Game_Board.Matrix + Und[2])=0;
        Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank].Score--;
        Deselect_Piece();
    }
}

//This function randomly selects a piece that can be moved and then moves it
int _destination[4],k,_move,_case=0;
void Random_Solve()
{
    int _nr_Curent_Movable_Pieces=0,_chose[Game_Board.Size.y*Game_Board.Size.x];
    bool _movable=false;
    srand(time(NULL));

    if(_case==0)
    {
        for(int i=0; i<Game_Board.Size.x*Game_Board.Size.y; i++)
        {
            _movable=false;
            if(*(Game_Board.Matrix + i)==1||*(Game_Board.Matrix + i)==2)
            {

                if(i+2*Game_Board.Size.x<=Game_Board.Size.x*Game_Board.Size.y &&(*(Game_Board.Matrix + i+2*Game_Board.Size.x)==0||*(Game_Board.Matrix + i+2*Game_Board.Size.x)==3)&&(*(Game_Board.Matrix + i+Game_Board.Size.x)==1||*(Game_Board.Matrix + i+Game_Board.Size.x)==2))
                {
                    _movable=true;
                }
                if(i-2*Game_Board.Size.x >=0 &&(*(Game_Board.Matrix + i-2*Game_Board.Size.x)==0||*(Game_Board.Matrix + i-2*Game_Board.Size.x)==3)&&(*(Game_Board.Matrix + i-Game_Board.Size.x)==1||*(Game_Board.Matrix + i-Game_Board.Size.x)==2))
                {
                    _movable=true;
                }

                if((i+2)/Game_Board.Size.x ==i/Game_Board.Size.x &&(*(Game_Board.Matrix + i+2)==0||*(Game_Board.Matrix + i+2)==3)&&(*(Game_Board.Matrix + i+1)==1||*(Game_Board.Matrix + i+1)==2))
                {
                    _movable=true;
                }

                if((i-2)/Game_Board.Size.x ==i/Game_Board.Size.x &&(*(Game_Board.Matrix + i-2)==0||*(Game_Board.Matrix + i-2)==3)&&(*(Game_Board.Matrix + i-1)==1||*(Game_Board.Matrix + i-1)==2))
                {
                    _movable=true;
                }
            }
            if(_movable)
            {
                _chose[_nr_Curent_Movable_Pieces]=i;
                _nr_Curent_Movable_Pieces++;
            }



        }
        Selected_Tile=_chose[rand()% _nr_Curent_Movable_Pieces];

        k=0;
        if(*(Game_Board.Matrix + Selected_Tile) == 1)
        {

            *(Game_Board.Matrix + Selected_Tile) = 2;



            if(Selected_Tile+2*Game_Board.Size.x<=Game_Board.Size.x*Game_Board.Size.y &&*(Game_Board.Matrix + Selected_Tile+2*Game_Board.Size.x)==0&&*(Game_Board.Matrix + Selected_Tile+Game_Board.Size.x)==1)
            {
                *(Game_Board.Matrix + Selected_Tile+2*Game_Board.Size.x)=3;
                _destination[k++]=1;
            }
            if(Selected_Tile-2*Game_Board.Size.x >=0 &&*(Game_Board.Matrix + Selected_Tile-2*Game_Board.Size.x)==0&&*(Game_Board.Matrix + Selected_Tile-Game_Board.Size.x)==1)
            {
                *(Game_Board.Matrix + Selected_Tile-2*Game_Board.Size.x)=3;
                _destination[k++]=2;
            }
            if((Selected_Tile+2)/Game_Board.Size.x ==Selected_Tile/Game_Board.Size.x &&*(Game_Board.Matrix + Selected_Tile+2)==0&&*(Game_Board.Matrix + Selected_Tile+1)==1)
            {
                *(Game_Board.Matrix + Selected_Tile+2)=3;
                _destination[k++]=3;
            }
            if((Selected_Tile-2)/Game_Board.Size.x ==Selected_Tile/Game_Board.Size.x &&*(Game_Board.Matrix + Selected_Tile-2)==0&&*(Game_Board.Matrix + Selected_Tile-1)==1)
            {
                *(Game_Board.Matrix + Selected_Tile-2)=3;
                _destination[k++]=4;
            }

        }
        _case=1;
    }
    else
    {


        _move=_destination[rand()% k];
        switch (_move)
        {
        case 1:

            *(Game_Board.Matrix + Selected_Tile+2*Game_Board.Size.x)=1;
            *(Game_Board.Matrix + Selected_Tile+Game_Board.Size.x)=0;
            *(Game_Board.Matrix + Selected_Tile)=0;

            Und[2]=Selected_Tile+2*Game_Board.Size.x;
            Und[1]=Selected_Tile+Game_Board.Size.x;
            Und[0]=Selected_Tile;
            break;
        case 2:
            *(Game_Board.Matrix + Selected_Tile-2*Game_Board.Size.x)=1;
            *(Game_Board.Matrix + Selected_Tile-Game_Board.Size.x)=0;
            *(Game_Board.Matrix + Selected_Tile)=0;

            Und[2]=Selected_Tile-2*Game_Board.Size.x;
            Und[1]=Selected_Tile-Game_Board.Size.x;
            Und[0]=Selected_Tile;
            break;

        case 3:
            *(Game_Board.Matrix + Selected_Tile+2)=1;
            *(Game_Board.Matrix + Selected_Tile+1)=0;
            *(Game_Board.Matrix + Selected_Tile)=0;

            Und[2]=Selected_Tile+2;
            Und[1]=Selected_Tile+1;
            Und[0]=Selected_Tile;
            break;

        case 4:
            *(Game_Board.Matrix + Selected_Tile-2)=1;
            *(Game_Board.Matrix + Selected_Tile-1)=0;
            *(Game_Board.Matrix + Selected_Tile)=0;

            Und[2]=Selected_Tile-2;
            Und[1]=Selected_Tile-1;
            Und[0]=Selected_Tile;
            break;
        }
        _case=0;
        Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank].Score++;
        Deselect_Piece();
        End_Game=Game_Over();
        Enable_Undo=true;
    }




}

//This function determines the best way to solve the game board, using the backtracking method
bool Done=false;
void Bkt_Solve()
{

    if(Step > Best)
    {
        Best=Step;
        for(int i=1 ; i<=Best ; i++)
        {
            Solution[i].Start=Temporary[i].Start;
            Solution[i].Middle=Temporary[i].Middle;
            Solution[i].End=Temporary[i].End;
        }
        if(Best == No_Of_Pieces-1)
            Done=true;

    }

    if(!Done)
        for(int i=0; i<Game_Board.Size.x*Game_Board.Size.y; i++)
        {
            if(*(Game_Board.Matrix + i)==1||*(Game_Board.Matrix + i)==2)
            {

                if(i+2*Game_Board.Size.x<=Game_Board.Size.x*Game_Board.Size.y &&(*(Game_Board.Matrix + i+2*Game_Board.Size.x)==0||*(Game_Board.Matrix + i+2*Game_Board.Size.x)==3)&&(*(Game_Board.Matrix + i+Game_Board.Size.x)==1||*(Game_Board.Matrix + i+Game_Board.Size.x)==2))
                {

                    Step++;
                    Temporary[Step].Start=i;
                    Temporary[Step].Middle=i+Game_Board.Size.x;
                    Temporary[Step].End=i+2*Game_Board.Size.x;

                    *(Game_Board.Matrix + i+2*Game_Board.Size.x) = 1;
                    *(Game_Board.Matrix + i+Game_Board.Size.x) = 0;
                    *(Game_Board.Matrix + i) = 0;

                    Bkt_Solve();

                    *(Game_Board.Matrix + i+2*Game_Board.Size.x) = 0;
                    *(Game_Board.Matrix + i+Game_Board.Size.x) = 1;
                    *(Game_Board.Matrix + i) = 1;
                    Step--;

                }
                if(i-2*Game_Board.Size.x >=0 &&(*(Game_Board.Matrix + i-2*Game_Board.Size.x)==0||*(Game_Board.Matrix + i-2*Game_Board.Size.x)==3)&&(*(Game_Board.Matrix + i-Game_Board.Size.x)==1||*(Game_Board.Matrix + i-Game_Board.Size.x)==2))
                {

                    Step++;
                    Temporary[Step].Start=i;
                    Temporary[Step].Middle=i-Game_Board.Size.x;
                    Temporary[Step].End=i-2*Game_Board.Size.x;

                    *(Game_Board.Matrix + i-2*Game_Board.Size.x) = 1;
                    *(Game_Board.Matrix + i-Game_Board.Size.x) = 0;
                    *(Game_Board.Matrix + i) = 0;

                    Bkt_Solve();

                    *(Game_Board.Matrix + i-2*Game_Board.Size.x) = 0;
                    *(Game_Board.Matrix + i-Game_Board.Size.x) = 1;
                    *(Game_Board.Matrix + i) = 1;
                    Step--;

                }

                if((i+2)/Game_Board.Size.x ==i/Game_Board.Size.x &&(*(Game_Board.Matrix + i+2)==0||*(Game_Board.Matrix + i+2)==3)&&(*(Game_Board.Matrix + i+1)==1||*(Game_Board.Matrix + i+1)==2))
                {

                    Step++;
                    Temporary[Step].Start=i;
                    Temporary[Step].Middle=i+1;
                    Temporary[Step].End=i+2;

                    *(Game_Board.Matrix + i+2) = 1;
                    *(Game_Board.Matrix + i+1) = 0;
                    *(Game_Board.Matrix + i) = 0;

                    Bkt_Solve();

                    *(Game_Board.Matrix + i+2) = 0;
                    *(Game_Board.Matrix + i+1) = 1;
                    *(Game_Board.Matrix + i) = 1;
                    Step--;

                }

                if((i-2)/Game_Board.Size.x ==i/Game_Board.Size.x &&(*(Game_Board.Matrix + i-2)==0||*(Game_Board.Matrix + i-2)==3)&&(*(Game_Board.Matrix + i-1)==1||*(Game_Board.Matrix + i-1)==2))
                {
                    Step++;
                    Temporary[Step].Start=i;
                    Temporary[Step].Middle=i-1;
                    Temporary[Step].End=i-2;

                    *(Game_Board.Matrix + i-2) = 1;
                    *(Game_Board.Matrix + i-1) = 0;
                    *(Game_Board.Matrix + i) = 0;

                    Bkt_Solve();

                    *(Game_Board.Matrix + i-2) = 0;
                    *(Game_Board.Matrix + i-1) = 1;
                    *(Game_Board.Matrix + i) = 1;
                    Step--;
                }
            }

        }

}

//This function controls the board and its variables
void Board_Decoder()
{


    if(*( Game_Board.Matrix + (((slGetMouseY() - (Window_UI_Vector[Game_Board.UI_Vector_Position].Center.y - (Game_Board.Size.y*Game_Board.Tile_Size)/2))/Game_Board.Tile_Size)*Game_Board.Size.x +  ((slGetMouseX()-(Window_UI_Vector[Game_Board.UI_Vector_Position].Center.x - (Game_Board.Size.x*Game_Board.Tile_Size)/2))/Game_Board.Tile_Size)))==3)
    {

        int _check=(((slGetMouseY() - (Window_UI_Vector[Game_Board.UI_Vector_Position].Center.y - (Game_Board.Size.y*Game_Board.Tile_Size)/2))/Game_Board.Tile_Size)*Game_Board.Size.x +  ((slGetMouseX()-(Window_UI_Vector[Game_Board.UI_Vector_Position].Center.x - (Game_Board.Size.x*Game_Board.Tile_Size)/2))/Game_Board.Tile_Size));
        if(_check+2*Game_Board.Size.x<=Game_Board.Size.x*Game_Board.Size.y &&*(Game_Board.Matrix + _check+2*Game_Board.Size.x)==2&&*(Game_Board.Matrix + _check+Game_Board.Size.x)==1)
        {

            *(Game_Board.Matrix + _check+2*Game_Board.Size.x)=0;
            *(Game_Board.Matrix + _check+Game_Board.Size.x)=0;
            *(Game_Board.Matrix + _check)=1;

            Und[0]= _check+2*Game_Board.Size.x;
            Und[1]= _check+Game_Board.Size.x;
            Und[2]= _check;
            Enable_Undo=true;
        }

        else
        {
            if(_check-2*Game_Board.Size.x >=0 &&*(Game_Board.Matrix + _check-2*Game_Board.Size.x)==2&&*(Game_Board.Matrix + _check-Game_Board.Size.x)==1)
            {
                *(Game_Board.Matrix + _check-2*Game_Board.Size.x)=0;
                *(Game_Board.Matrix + _check-Game_Board.Size.x)=0;
                *(Game_Board.Matrix + _check)=1;

                Und[0]= _check-2*Game_Board.Size.x;
                Und[1]= _check-Game_Board.Size.x;
                Und[2]= _check;
                Enable_Undo=true;
            }
            else
            {
                if((_check+2)/Game_Board.Size.x ==_check/Game_Board.Size.x &&*(Game_Board.Matrix + _check+2)==2&&*(Game_Board.Matrix + _check+1)==1)
                {
                    *(Game_Board.Matrix + _check+2)=0;
                    *(Game_Board.Matrix + _check+1)=0;
                    *(Game_Board.Matrix + _check)=1;

                    Und[0]= _check+2;
                    Und[1]= _check+1;
                    Und[2]= _check;
                    Enable_Undo=true;
                }
                else
                {
                    if((_check-2)/Game_Board.Size.x ==_check/Game_Board.Size.x &&*(Game_Board.Matrix + _check-2)==2&&*(Game_Board.Matrix + _check-1)==1)
                    {
                        *(Game_Board.Matrix + _check-2)=0;
                        *(Game_Board.Matrix + _check-1)=0;
                        *(Game_Board.Matrix + _check)=1;

                        Und[0]= _check-2;
                        Und[1]= _check-1;
                        Und[2]= _check;
                        Enable_Undo=true;
                    }
                }
            }
        }
        End_Game=Game_Over();
        Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank].Score++;

    }

    Deselect_Piece();

    if(Selected_Tile!=(((slGetMouseY() - (Window_UI_Vector[Game_Board.UI_Vector_Position].Center.y - (Game_Board.Size.y*Game_Board.Tile_Size)/2))/Game_Board.Tile_Size)*Game_Board.Size.x +  ((slGetMouseX()-(Window_UI_Vector[Game_Board.UI_Vector_Position].Center.x - (Game_Board.Size.x*Game_Board.Tile_Size)/2))/Game_Board.Tile_Size)))
    {

        Selected_Tile = (((slGetMouseY() - (Window_UI_Vector[Game_Board.UI_Vector_Position].Center.y - (Game_Board.Size.y*Game_Board.Tile_Size)/2))/Game_Board.Tile_Size)*Game_Board.Size.x +  ((slGetMouseX()-(Window_UI_Vector[Game_Board.UI_Vector_Position].Center.x - (Game_Board.Size.x*Game_Board.Tile_Size)/2))/Game_Board.Tile_Size));

        Select_Piece();


    }
    else
    {
        Selected_Tile = -1;
    }

}

//This function initializes and executes all the games functions
void Game_Window()
{

    Load_Options("files/Options_File.txt");

    Temp_Game_Settings = Game_Settings;

    slWindow(Game_Settings.Size.x,Game_Settings.Size.y,"Solitarul",Game_Settings.Fullscreen);

    Game_Board.Matrix = Menu_Board_Int_Array;
    Load_Board(Board_Address_Array[0]);

    slSetTextAlign(SL_ALIGN_CENTER);
    slSetFont(slLoadFont("files/font.TTF"),Game_Settings.UI_Scalar_Unit/1.5);

    Load_UI("files/Window_UI.txt");

    Bkt_Solve();

    while(!slShouldClose() && !slGetKey(SL_KEY_ESCAPE) && !Window_Closed)
    {

        Last_Key_Pressed = Return_Key();

        Draw_UI();

        if(String_Contains(Last_Key_Pressed,"SL_MOUSE_BUTTON_LEFT"))
        {

            if(Find_UI_Element_Tag() == "Game_Board" && Current_Window_UI_Tag == "Game")
            {

                if(!End_Game)
                {

                    Board_Decoder();

                }

            }
            else if(Find_UI_Element_Tag() != "")
            {

                UI_Decoder(Find_UI_Element_Tag());

            }
            else
            {

                Deselect_Piece();
                Selected_Tile=-1;

            }

            Has_Key_Been_Pressed = true;

        }

        if(Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank].Score > Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank - 1].Score && Game_Board.Current_Score_Rank > 0)
        {

            Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank - 1] = Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank];

            Game_Board.Current_Score_Rank--;

            for(int i = 5; i > Game_Board.Current_Score_Rank; i--)
            {

                Game_Board.Current_Score_Board[i] = Game_Board.Score_Board[i - 1];

            }

        }

        if(End_Game && Take_Input_From_Keyboard && Current_Window_UI_Tag == "Game" && Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank].Initial.length() < 3)
        {

            Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank].Initial += ((Last_Key_Pressed.find("<") != UINT_MAX) ? "" : Last_Key_Pressed);

        }
        else if(End_Game && Current_Window_UI_Tag == "Game" && Game_Board.Current_Score_Board[Game_Board.Current_Score_Rank].Initial.length() >= 3)
        {

            Take_Input_From_Keyboard = false;

        }

        if(End_Game && Current_Window_UI_Tag == "Game" && !Take_Input_From_Keyboard && !Has_Written_To_File)
        {

            for(int i = 0; i < 5; i++)
            {

                Write_Value(Board_Address_Array[Current_Board_Index],(to_string(i) + "s"),"<Score>" + to_string(Game_Board.Current_Score_Board[i].Score) + "</Score>");
                Write_Value(Board_Address_Array[Current_Board_Index],(to_string(i) + "i"),"<Initial>" + Game_Board.Current_Score_Board[i].Initial + "</Initial>");
                Write_Value(Board_Address_Array[Current_Board_Index],(to_string(i) + "t"),"<Time>" + to_string(Game_Board.Current_Score_Board[i].Time) + "</Time>");

                Game_Board.Score_Board[i] = Game_Board.Current_Score_Board[i];

            }

            Has_Written_To_File = true;

        }

    }

    slClose();

}




int main()
{

    Game_Window();

    return 0;
}
