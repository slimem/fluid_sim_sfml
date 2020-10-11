#include <SFML/Graphics.hpp>
#include "FluidDomain.h"
#include <cstdlib>
#include <ctime>
#include <sstream>

using namespace std;

int main()
{
    srand(time(nullptr));
    const unsigned int FPS = 30;
    sf::RenderWindow window(sf::VideoMode(N*SCALE, N*SCALE), "Fluid sim");
    window.setFramerateLimit(FPS);

    float diffusion = 0.2f;
    float viscosity = 0.f;
    float timestep = 0.000001;
    FluidDomain fluid(diffusion, viscosity, timestep);
    bool addFluid = false;
    bool paused = false;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();

        // I use the following section to experiment with the fluid
#if 0
        for (int i = 0; i < 2; i++) {
            fluid.addVelocity(1, 1, 0, 50000);
        }
        int cx = int(0.5 * N / SCALE);
        int cy = int(0.5 * N / SCALE);
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (addFluid) {
                    //fluid.addDensity(cx + i, cy + j, (rand() % 100) + 50);
                    fluid.addDensity(cx + i, cy + j, 150);
                }
            }
        }
#endif

        sf::Vector2i position = sf::Mouse::getPosition(window);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)) {
            addFluid = true;
        } else {
            addFluid = false;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            paused = !paused;
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (addFluid) {
                for (int i = -10; i < 10; ++i) {
                    for (int j = -10; j < 10; ++j) {
                        int ri = rand() % 20;
                        int rj = rand() % 20;
                        fluid.addDensity((position.x + i+ri) / SCALE, (position.y + j+rj) / SCALE, (rand() % 100));
                    }
                }
            }
            else {
                // This will add a constant velocity in one direction (bottom right). this can be improved
                // further to control the velocity in all directions
                fluid.addVelocity(position.x / SCALE, position.y / SCALE, 50000, 50000);
            }
        }
        
        // advance
        if (!paused)
        fluid.step();

        // rendering as point will assign a color to each individual pixel. It performs better but does not scale
        // well with the screen and rendering as a rectangle will assign the individual colors to rectangles.
#if 0
        fluid.renderAsPoint();
#else
        fluid.renderAsRect();
#endif
        for (const auto& ww : fluid.getRectMap()) {
            window.draw(ww);
        }

        sf::Font font;
        if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf"))
        {
            // error...
        }

        sf::Text text;
        {
            ostringstream os;
            os << "Diffusion: " << diffusion << endl;
            os << "Viscosity: " << viscosity << endl;
            os << "Timestep: " << timestep << endl;
            os << "Max Density at (" << position.x << "," << position.y << "): " << fluid.getMaxD() << endl;
            if (paused) {
                os << "--PAUSED--\n";
            }
            text.setString(os.str());
        }

        text.setFont(font);
        text.setFillColor(sf::Color::Red);
        //text.setStyle(sf::Text::Bold);
        text.setPosition(10, 10);
        text.setCharacterSize(16);
        window.draw(text);
        //window.draw(densityText);
        window.display();
    }

    return 0;
}