#ifndef SOLTRACE_DATETIME_H
#define SOLTRACE_DATETIME_H

// Dummy date-time class to stand in for future date-time representation

class Date
{
public:
    Date(){}
private:
};

class Time
{
public:
    Time(){}
private:
};

class DateTime
{
public:
    DateTime(){}
    const Date& get_date() const {return this->my_date;}
    void set_date(const Date &d){this->my_date = d; return;}
    const Time& get_time() const {return this->my_time;}
    void set_time(const Time &t) {this->my_time = t; return;}
    void set_datetime(const DateTime &dt)
    {
        this->set_date(dt.get_date());
        this->set_time(dt.get_time());
        return;
    }
private:
    Date my_date;
    Time my_time;
};

#endif
