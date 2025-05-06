#ifndef SOLTRACE_DATETIME_H
#define SOLTRACE_DATETIME_H

// Dummy date-time class to stand in for future date-time representation

struct Date
{
public:

    // TODO: Implement this struct

    Date() {}

private:
};

struct Time
{
public:

    // TODO: Implement this struct

    Time() {}

private:
};

struct DateTime
{
public:
    Date my_date;
    Time my_time;

    DateTime() {}

    void set(const DateTime &dt)
    {
        // this->set_date(dt.get_date());
        // this->set_time(dt.get_time());
        this->my_date = dt.my_date;
        this->my_time = dt.my_time;
        return;
    }

    DateTime &operator=(const DateTime &rhs)
    {
        this->set(rhs);
        return *this;
    }

private:
};

#endif
