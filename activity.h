#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QString>

class Activity
{
public:
    Activity();
    virtual double getCaloriesBurned()const = 0;
    virtual QString getSummary() const = 0;
};




class Running : public Activity{

};




#endif // ACTIVITY_H
