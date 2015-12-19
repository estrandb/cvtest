#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

class ServoController
{
    public:
        ServoController();
        void MovePanServo(int position);
        void MoveTiltServo(int position);


    protected:
    private:
        int uart0_filestream;
};

#endif // SERVOCONTROLLER_H
