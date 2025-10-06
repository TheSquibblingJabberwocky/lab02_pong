#include <sstream>
#include <SFML/Graphics.hpp>

const sf::Keyboard::Key controls[4] = {
    sf::Keyboard::Q,   
    sf::Keyboard::S,  
    sf::Keyboard::Up,  
    sf::Keyboard::Down 
};

//Parameters
const sf::Vector2f paddleSize(25.f, 100.f);
const float ballRadius = 10.f;
const int gameWidth = 800;
const int gameHeight = 600;
const float paddleSpeed = 400.f;
const float paddleOffsetWall = 10.f;
const float time_step = 0.017f; //60 fps

sf::Vector2f ballVelocity;
bool isPlayerServing = true;
const float initialVelocityX = 100.f;
const float initialVelocityY = 60.f;
const float velocityMultiplier = 1.1f;

int score[2] = { 0,0 };

//Objects of the game
sf::CircleShape ball;
sf::RectangleShape paddles[2];
sf::Text text;
sf::Font font;


void reset() {
    ballVelocity = { (isPlayerServing ? initialVelocityX : -initialVelocityX), initialVelocityY };

    // reset paddle position
    paddles[0].setPosition(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f);
    paddles[1].setPosition(gameWidth - paddleOffsetWall - paddleSize.x / 2.f, gameHeight / 2.f);
    // reset Ball Position in the centre of the left paddle
    ball.setPosition(paddleOffsetWall + paddleSize.x + ballRadius * 2.f, gameHeight / 2.f);

    // Update Score Text
    std::stringstream sstr;
    sstr << score[0] << " - " << score[1];
    text.setString(sstr.str());
    // Keep Score Text Centered
    text.setPosition((gameWidth * .5f) - (text.getLocalBounds().width * .5f), 0);
}

void init() {
    // Set size and origin of paddles
    for (sf::RectangleShape& p : paddles) {
        p.setSize(paddleSize);
        p.setOrigin(paddleSize / 2.f);
    }
    // Set size and origin of ball
    ball.setRadius(ballRadius);
    ball.setOrigin(ballRadius, ballRadius); //Should be half the ball width and height

    // Load font-face from res dir
    font.loadFromFile("resources/fonts/RobotoMono-Regular.ttf");
    // Set text element to use font
    text.setFont(font);
    // set the character size to 24 pixels
    text.setCharacterSize(24);
    text.setColor(sf::Color(255, 255, 255));
    reset();
}


void update(float dt) {


    // handle paddle movement
    float direction = 0.0f;
    if (sf::Keyboard::isKeyPressed(controls[0]) &&
        paddles[0].getPosition().y > paddleSize.y / 2.f) {
        direction--;
    }
    if (sf::Keyboard::isKeyPressed(controls[1]) &&
        paddles[0].getPosition().y < gameHeight - paddleSize.y / 2.f) {
        direction++;
    }

    paddles[0].move(sf::Vector2f(0.f, direction * paddleSpeed * dt));

    // handle paddle movement
    direction = 0.0f;
    if (sf::Keyboard::isKeyPressed(controls[2]) &&
        paddles[1].getPosition().y > paddleSize.y / 2.f) {
        direction--;
    }
    if (sf::Keyboard::isKeyPressed(controls[3]) &&
        paddles[1].getPosition().y < gameHeight - paddleSize.y / 2.f) {
        direction++;
    }

    paddles[1].move(sf::Vector2f(0.f, direction * paddleSpeed * dt));
    ball.move(ballVelocity * dt);

    // check ball collision
    const float bx = ball.getPosition().x;
    const float by = ball.getPosition().y;
    if (by > gameHeight) { //bottom wall
        // bottom wall
        ballVelocity.x *= velocityMultiplier;
        ballVelocity.y *= -velocityMultiplier;
        ball.move(sf::Vector2f(0.f, -10.f));
    }
    else if (by < 0) { //top wall
        // top wall
        ballVelocity.x *= velocityMultiplier;
        ballVelocity.y *= -velocityMultiplier;
        ball.move(sf::Vector2f(0.f, 10.f));
    }
    else if (bx > gameWidth) {
        // right wall
        score[0]++;
        reset();
    }
    else if (bx < 0) {
        // left wall
        score[1]++;
        reset();
    }
    else if (
        //ball is inline or behind paddle AND
        bx < paddleSize.x + paddleOffsetWall &&
        //ball is below top edge of paddle AND
        by > paddles[0].getPosition().y - (paddleSize.y * 0.5) &&
        //ball is above bottom edge of paddle
        by < paddles[0].getPosition().y + (paddleSize.y * 0.5)) {
        //bounces off left paddle
        ballVelocity.x *= -velocityMultiplier;
        ballVelocity.y *= velocityMultiplier;
        ball.move(sf::Vector2f(+10.f, 0));
    }
    else if (
        //ball is inline or behind paddle AND
        bx > gameWidth - paddleSize.x - paddleOffsetWall &&
        //ball is below top edge of paddle AND
        by > paddles[1].getPosition().y - (paddleSize.y * 0.5) &&
        //ball is above bottom edge of paddle
        by < paddles[1].getPosition().y + (paddleSize.y * 0.5)) {
        // bounce off right paddle
        ballVelocity.x *= -velocityMultiplier;
        ballVelocity.y *= velocityMultiplier;
        ball.move(sf::Vector2f(-10.f, 0));
    }
}



void render(sf::RenderWindow& window) {
    // Draw Everything
    window.draw(paddles[0]);
    window.draw(paddles[1]);
    window.draw(ball);
    window.draw(text);
}

int main() {
    sf::RenderWindow window(sf::VideoMode({ gameWidth, gameHeight }), "PONG");
    window.setVerticalSyncEnabled(true);
    init();
    //initialise and load
    while (window.isOpen()) {
        // Reset clock, recalculate deltatime
        static sf::Clock clock;
        float dt = clock.restart().asSeconds();
        // check and consume events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
        }
        // Quit Via ESC Key
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
        }

        window.clear();
        update(dt);
        render(window);
        //wait for the time_step to finish before displaying the next frame.
        sf::sleep(sf::seconds(time_step));
        //Wait for Vsync
        window.display();
    }
    return 0;
}