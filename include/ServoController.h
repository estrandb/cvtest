#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

class ServoController
{
    public:
        ServoController();
        void MovePanServoTo(int position);
        void MovePanServoBy(int offset);
        void MoveTiltServoTo(int position);
        void MoveTiltServoBy(int offset);

    protected:
    private:
        int uart0_filestream;
        int currentPanPosition;
        int currentTiltPosition;
};

#endif // SERVOCONTROLLER_H
