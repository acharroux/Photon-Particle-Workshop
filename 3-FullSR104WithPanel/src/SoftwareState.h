#ifndef _SOFTWARE_STATE_
#define _SOFTWARE_STATE_

class SoftwareState
{
public:
    SoftwareState(void);

    double getRange(void) const;
    unsigned long getNbRanges(void) const;
    unsigned long getNbValidRanges() const;
    unsigned long getNbCurrentValids() const;

    void setRange(double iRange);
    bool rangeIsValidForALongTime(void) const;

    // Used for cloud registration of the 'cm' variable
    const double &getRangeVariable(void) const;

    void trackEventReceived(void);
    void trackSentEvent(void);

    unsigned long getNbReceivedEvents(void) const;
    unsigned long getNbSentEvents(void) const;

private:
    double mRange;
    unsigned long mNbRanges;
    unsigned long mNbValidRanges;
    unsigned long mNbCurrentValids;
    unsigned long mNbReceivedEvents;
    unsigned long mNbSentEvents;

    bool isValidRange(double iRange) const;
};

#endif // _SOFTWARE_STATE_