#include "Profile.h"
#include "../Player.h"
#include "../data/ImageCenter.h"
#include "../data/DataCenter.h"
#include "../shapes/Point.h"
#include "allegro5/allegro_primitives.h"
#include <utility>
#include <vector>

// Screen dimensions (from DataCenter)
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// Monster display dimensions (consistent with other scenes)
const int MONSTER_WIDTH = 80;
const int MONSTER_HEIGHT = 150;

// Grid layout configuration
const int MONSTERS_PER_ROW = 4;
const int MAX_ROWS = 2;
const int MAX_MONSTERS_DISPLAYED = MONSTERS_PER_ROW * MAX_ROWS;

// Calculate spacing and margins for centered grid
const int HORIZONTAL_SPACING = 220;  // Space between monsters horizontally
const int VERTICAL_SPACING = 200;    // Space between monsters vertically
const int GRID_WIDTH = (MONSTERS_PER_ROW - 1) * HORIZONTAL_SPACING + MONSTER_WIDTH;
const int GRID_HEIGHT = (MAX_ROWS - 1) * VERTICAL_SPACING + MONSTER_HEIGHT;
const int START_X = (SCREEN_WIDTH - GRID_WIDTH) / 2;
const int START_Y = (SCREEN_HEIGHT - GRID_HEIGHT) / 2 + 20;  // Slightly below center

// Function to calculate monster position based on index
std::pair<int, int> getMonsterPosition(int index) {
    int row = index / MONSTERS_PER_ROW;
    int col = index % MONSTERS_PER_ROW;
    int x = START_X + col * HORIZONTAL_SPACING;
    int y = START_Y + row * VERTICAL_SPACING;
    return {x, y};
}

// Helper function to get button image with hover effect
static std::string getButtonImage(const std::string& basePath, bool hovering) {
    if (!hovering) return basePath;
    size_t dotPos = basePath.rfind(".png");
    if (dotPos != std::string::npos) {
        return basePath.substr(0, dotPos) + "2.png";
    }
    return basePath;
}

void Profile::init(){

}

void Profile::update(){
    auto pl = Player::getPlayer();
    auto DC = DataCenter::get_instance();
    //exit  
    if(Point(1100, 10).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
        pl->setrequest(Game::STATE::MENU);
    }
}

void Profile::draw(){
    Player *pl = Player::getPlayer();
    std::vector<Monster> lib = pl->getAllMonsters();
    auto IC = ImageCenter::get_instance();
    auto DC = DataCenter::get_instance();
    auto bg = IC->get("./assets/image/scene/pfp.png");

    //background
    al_draw_bitmap(bg, 0, 0, 0);

    // Draw monsters in a grid layout
    int monstersOwned = pl->getMonsters().size();
    int monstersToDisplay = (monstersOwned < MAX_MONSTERS_DISPLAYED) ? monstersOwned : MAX_MONSTERS_DISPLAYED;
    
    for(int i = 0; i < monstersToDisplay; i++){
        auto pos = getMonsterPosition(i);
        auto img = pl->getMonsters()[i].getImgInPfp();
        al_draw_bitmap(img, pos.first, pos.second, 0);
        
        // Optional: draw hitboxes for debugging
        // al_draw_rectangle(pos.first, pos.second, 
        //                   pos.first + MONSTER_WIDTH, pos.second + MONSTER_HEIGHT, 
        //                   al_map_rgb(255, 0, 0), 2);
    }

    //exit
    bool exitHover = Point(1100, 10).overlap(DC->mouse, 40);
    auto exit = IC->get(getButtonImage("./assets/image/littleStuff/exit.png", exitHover));
    al_draw_bitmap(exit, 1100, 10, 0);

    //hitboxes
    al_draw_circle(1100, 10, 40, al_map_rgb(255,0,0), 2);
}
void Profile::end(){
    
}