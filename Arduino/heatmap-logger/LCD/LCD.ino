#include <LcdMenu.h>
#include <MenuItem.h>

#define LCD_ROWS 2
#define LCD_COLS 16

// Initialize the main menu items
MenuItem item0 = MenuItem("item0");
MenuItem item1 = MenuItem("item1");
MenuItem item2 = MenuItem("item2");
MenuItem item3 = MenuItem("item3");
MenuItem item4 = MenuItem("item4");

MAIN_MENU(
    &item0,
    &item1,
    &item2,
    &item3,
    &item4
);
// Construct the LcdMenu
LcdMenu menu(LCD_ROWS, LCD_COLS);

void setup() {
    Serial.begin(9600);
    // Initialize LcdMenu with the menu items
    menu.setupLcdWithMenu(0x27, mainMenu);
}

void loop() {
    item0.setText("test");
    menu.update();
    for (int i=0; i<5; i++){
      delay(1000);
      menu.down();
    }
    for (int i=0; i<5; i++){
      delay(1000);
      menu.up();
    }
}