#ifndef _PANEL_

#define _PANEL_

/** simple abstract class to manage the display of a panel */
/** forward declaration **/

class Adafruit_SSD1306;

class Panel
{
public:
    typedef enum
    {
        SmallFont = 1,
        LargeFont = 2
    } eFont;

    /** panel will disply its content in a 128x64 oled display */
    Panel(Adafruit_SSD1306 &iDisplay);
    virtual ~Panel(void);

    /** 
    * optional : it can be mandatory to do some init after actual connection to the cloud
    * overwrite this method to do it 
    * default implementation will do nothing */

    virtual void initAfterConnect(void);

    /** 
    * optional : it can be more efficient to keep track of values to display then display if one has changed
    * than always fetch AND display the values 
    * overwrite this method if needed
    * default implementation will always display the values
    */

    virtual bool refreshValues(void);

    virtual void display(void) = 0;

protected:
    void resetDisplay(void);
    void setFont(eFont iFontSize);
    eFont getFont(void) const;
    void setVPos(int iVPos);
    int getVPos(void) const;
    void newLine(void);
    Adafruit_SSD1306 &getDisplay(void) const;

private:
    Adafruit_SSD1306 &mDisplay;
    int mVPos;
    eFont mFont;

    Panel(void);
    int getFontSize(void) const;
};

#endif // _PANEL_