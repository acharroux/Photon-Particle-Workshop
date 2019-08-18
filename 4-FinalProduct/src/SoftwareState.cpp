#include "application.h"
#include "SoftwareState.h"

// here are some undocumented global variables quite useful

extern uint16_t __system_product_id;
extern uint16_t __system_product_version;

/*********************************************/

SoftwareState::SoftwareState(void) : mRange(0.0),
                                     mNbRanges(0),
                                     mNbValidRanges(0),
                                     mNbCurrentValids(0),
                                     mNbReceivedEvents(0),
                                     mNbSentEvents(0)
{
}

/*********************************************/

unsigned SoftwareState::getProductID(void) const
{
    return static_cast<unsigned>(__system_product_id);
}

/*********************************************/

unsigned SoftwareState::getProductVersion(void) const
{
    return static_cast<unsigned>(__system_product_version);
}

/*********************************************/

double SoftwareState::getRange(void) const
{
    return mRange;
}

/*********************************************/

unsigned long SoftwareState::getNbRanges(void) const
{
    return mNbRanges;
}

/*********************************************/

unsigned long SoftwareState::getNbValidRanges(void) const
{
    return mNbValidRanges;
}

/*********************************************/

unsigned long SoftwareState::getNbCurrentValids(void) const
{
    return mNbCurrentValids;
}

/*********************************************/

void SoftwareState::setRange(double iRange)
{
    mRange = iRange;
    // keep tracks of what we have done
    mNbRanges++;
    if (isValidRange(iRange))
    {
        mNbValidRanges++;
        mNbCurrentValids++;
    }
    else
    {
        mNbCurrentValids = 0;
    }
}

/*********************************************/

bool SoftwareState::isValidRange(double iRange) const
{
    return (iRange >= 15) && (iRange <= 25);
}

/*********************************************/
// return true every 15 valid ranges in sequence ie
// roughly every second with the delay in our current loop

bool SoftwareState::rangeIsValidForALongTime(void) const
{
    return (mNbCurrentValids > 10) && (mNbCurrentValids % 10 == 0);
}

/*********************************************/

const double &SoftwareState::getRangeVariable(void) const
{
    return mRange;
}

/*********************************************/

void SoftwareState::trackEventReceived(void)
{
    mNbReceivedEvents++;
}

/*********************************************/

void SoftwareState::trackSentEvent(void)
{
    mNbSentEvents++;
}

/*********************************************/

unsigned long SoftwareState::getNbReceivedEvents(void) const
{
    return mNbReceivedEvents;
}

/*********************************************/

unsigned long SoftwareState::getNbSentEvents(void) const
{
    return mNbSentEvents;
}