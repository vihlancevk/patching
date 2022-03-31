#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <cassert>
#include <thread>
#include <chrono>

enum ErrorCode
{
    NO_ERROR,
    NO_FONT_FILE,
    NO_IMAGE_FILE,
    NO_PATCH_FILE,
    FSEEK_ERROR
};

ErrorCode PatchFile( FILE *finput);
ErrorCode RunPatching( sf::Sprite backgroundImage, sf::Sprite buttonImage, sf::Text buttonText, sf::RectangleShape progressBar, sf::Text progressBarText );

const double SCREEN_LENGTH = 1920;
const double SCREEN_WIDTH  = 1080;

const double X_BUTTON_COORDINATE = 45    ;
const double Y_BUTTON_COORDINATE = 538.75;

const double X_TEXT_BUTTON_COORDINATE = 300;
const double Y_TEXT_BUTTON_COORDINATE = 650;

const double X_PROGRESS_BAR_COORDINATE = 180;
const double Y_PROGRESS_BAR_COORDINATE = 750;

const double Y_PROGRESS_BAR_TEXT_COORDINATE = 746;

const char* FILE_FOR_PATCH         = "../PSWRD21.COM"   ;
const char* FILE_FONT              = "images/font.ttf"  ;
const char* FILE_IMAGE_BACK_GROUND = "images/rkn.jpeg"  ;
const char* FILE_IMAGE_BUTTON      = "images/button.png";

int main()
{
    ErrorCode errorCode = NO_ERROR;

    sf::Font font;
    if ( !font.loadFromFile( FILE_FONT ) )
    {
        std::cerr << "Can't find the font file" << std::endl;
        errorCode = NO_FONT_FILE;
    }

    sf::Texture background;
    sf::Sprite backgroundImage;
    if ( !background.loadFromFile( FILE_IMAGE_BACK_GROUND ) )
    {
        std::cerr << "Couldn`t display image" << std::endl;
        errorCode = NO_IMAGE_FILE;
    }
    backgroundImage.setTexture( background, true );
    sf::Vector2f targetSize( SCREEN_LENGTH, SCREEN_WIDTH );
    float zoomX = targetSize.x / backgroundImage.getLocalBounds().width;
    float zoomY = targetSize.y / backgroundImage.getLocalBounds().height;
    backgroundImage.setScale( zoomX, zoomY );

    sf::Texture button;
    sf::Sprite buttonImage;
    if ( !button.loadFromFile( FILE_IMAGE_BUTTON ) )
    {
        std::cerr << "Can't find the image" << std::endl;
        errorCode = NO_IMAGE_FILE;
    }
    buttonImage.setPosition( X_BUTTON_COORDINATE, Y_BUTTON_COORDINATE );
    buttonImage.setScale( zoomX, zoomY );
    buttonImage.setTexture( button );

    sf::Text buttonText;
    buttonText.setFont( font );
    buttonText.setStyle( sf::Text::Bold );
    buttonText.setString( "Hacking" );
    buttonText.setFillColor( sf::Color::Black );
    buttonText.setCharacterSize( 100 );
    buttonText.setPosition( X_TEXT_BUTTON_COORDINATE, Y_TEXT_BUTTON_COORDINATE );

    sf::RectangleShape progressBar;
    progressBar.setFillColor(sf::Color::Blue);
    progressBar.setOutlineThickness( 3 );
    progressBar.setPosition( X_PROGRESS_BAR_COORDINATE, Y_PROGRESS_BAR_COORDINATE );
    progressBar.setOutlineColor(sf::Color::Black);

    sf::Text progressBarText;
    progressBarText.setFont( font );
    progressBarText.setStyle( sf::Text::Bold );
    progressBarText.setFillColor( sf::Color::Black );
    progressBarText.setCharacterSize( 40 );

    if ( !errorCode )
        errorCode = RunPatching( backgroundImage, buttonImage, buttonText, progressBar, progressBarText );

  return errorCode;
}

ErrorCode PatchFile( FILE *finput)
{
    if( finput == nullptr )
        return NO_PATCH_FILE;

    unsigned char isAccess = 0;
    unsigned int position  = 157;

    if ( fseek ( finput, position, SEEK_SET ) )
    {
        std::cerr << "The pointer to the file position has not been moved" << std::endl;
        return FSEEK_ERROR;
    }

    fscanf( finput, "%c", &isAccess );

    if ( isAccess )
    {
        std::cerr << "File already patching" << std::endl;
    }
    else
    {
        isAccess = 1;

        if ( fseek ( finput, position, SEEK_SET ) )
        {
            std::cerr << "The pointer to the file position has not been moved" << std::endl;
            return FSEEK_ERROR;
        }

        fwrite( &isAccess, sizeof( char ), 1, finput );

        std::cout << "File successful patching" << std::endl;
    }

    return NO_ERROR;
}

ErrorCode RunPatching( sf::Sprite backgroundImage, sf::Sprite buttonImage, sf::Text buttonText, sf::RectangleShape progressBar, sf::Text progressBarText )
{
    ErrorCode errorCode = NO_ERROR;
    sf::RenderWindow window( sf::VideoMode( SCREEN_LENGTH, SCREEN_WIDTH ), "", sf::Style::Default );

    while ( window.isOpen() )
    {
        sf::Event Event;
        while ( window.pollEvent( Event ) )
        {
            switch ( Event.type )
            {
                case sf::Event::Closed:
                {
                    window.close();
                    break;
                }
                case sf::Event::MouseMoved:
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition( window );
                    sf::Vector2f mousePosF( static_cast<float>( mousePos.x ), static_cast<float>( mousePos.y ) );
                    if ( buttonImage.getGlobalBounds().contains( mousePosF ) )
                    {
                        buttonImage.setColor( sf::Color( 250, 20, 20 ) );
                    }
                    else
                    {
                        buttonImage.setColor( sf::Color( 255, 255, 255 ) );
                    }
                    break;
                }
                case sf::Event::MouseButtonPressed:
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition( window );
                    sf::Vector2f mousePosF( static_cast<float>( mousePos.x ), static_cast<float>( mousePos.y ) );
                    if ( buttonImage.getGlobalBounds().contains( mousePosF ) )
                    {
                        int n_percents = 100;
                        FILE *finput = fopen( FILE_FOR_PATCH, "r+b" );
                        errorCode = PatchFile( finput );
                        while ( n_percents > 1 )
                        {
                            int PROGRESS_BAR_LENGTH = (100 - n_percents) * 7;
                            progressBar.setSize( sf::Vector2f( PROGRESS_BAR_LENGTH, 40) );
                            double X_PROGRESS_BAR_TEXT_COORDINATE = 150 + (100 - n_percents) * 7 / 2;
                            progressBarText.setPosition( X_PROGRESS_BAR_TEXT_COORDINATE, Y_PROGRESS_BAR_TEXT_COORDINATE );
                            progressBarText.setString( std::to_string(100 - n_percents) + "%" );
                            window.clear();
                            window.draw( backgroundImage );
                            window.draw( progressBar );
                            window.draw( progressBarText );
                            window.display();
                            n_percents--;
                            std::this_thread::sleep_for (std::chrono::milliseconds(100));
                        }
                        fclose( finput );
                        system ( "vlc -f --no-video-title-show --play-and-exit videos/rkn.mp4" );
                        window.close();
                    }
                    break;
                }
            }
        }

    window.clear();
    window.draw( backgroundImage );
    window.draw( buttonImage );
    window.draw( buttonText );
    window.display();
    }

    return errorCode;
}
