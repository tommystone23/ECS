#ifndef SYSTEM_H
#define SYSTEM_H

class ECS;

class System
{
public:
    System(ECS *ecs);
    virtual ~System(){}

    virtual void run_system(float delta_time) = 0;

protected:
    ECS *_ecs;
};

#endif // SYSTEM_H