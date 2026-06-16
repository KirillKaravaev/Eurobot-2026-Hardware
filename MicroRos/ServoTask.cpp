#include "ServoTask.h"
#include "SCServo.h"
#include "usart.h"
#include "cmsis_os.h"

extern uint8_t servo_data;
extern bool servo_spin_flag;

HardwareSerial SerialServo(UART4, 1000000); // 115200
SMS_STS sms_sts;

int servo_close_states[4] = { 1, 1, 1, 1 };
int servo_open_states[4] = { 0, 0, 0, 0 };

int servo_states[4] = { 0 };
int servo_payload[4] = { 0, 0, 0, 0 };

static void set_grippers(int* states) {
    u8 servos_ids[] = { 1, 2, 3, 4 };
    u16 speed[] = { 3400, 3400, 3400, 3400 };
    u8 acc[] = { 50, 50, 50, 50 };
    s16 position[4];

    for (int i = 0; i < 4; i++) {
        if (states[i])
            position[i] = 880;
        else
            position[i] = 2048;
    }

    sms_sts.SyncWritePosEx(servos_ids, 4, position, speed, acc);
}

static void set_height(s16 pos) {
    u8 servos_ids[] = { 6, 7 };
    u16 speed[] = { 3400, 3400 };
    u8 acc[] = { 25, 25 };
    s16 position[2];

    position[0] = 2047 + pos;
    position[1] = 2047 - pos;
    sms_sts.SyncWritePosEx(servos_ids, 2, position, speed, acc);
}

static void go_down() {
    int pos_down = -1550;
    set_height(pos_down);
}

void ServoInit() {
    SerialServo.begin();
    sms_sts.pSerial = &SerialServo;
}

void ServoSpin() {
    uint8_t command = (servo_data & 0xF0) >> 4;

    if (servo_spin_flag == true) {

        if (command == 0) {
            // взятие
            set_grippers(servo_open_states);
            sms_sts.WritePosEx(5, 1250, 3400, 50); // 1900 - mid, 1250 - open
            osDelay(300);
            go_down();
            osDelay(1300);
            sms_sts.WritePosEx(5, 1900, 3400, 50);

            osDelay(300);
            set_grippers(servo_close_states);
            osDelay(800);
            set_height(500);

            for (int i = 0; i < 4; i++) {
                servo_states[i] = ((servo_data & (1 << i)) >> i);
                servo_payload[i] = 1;
            }
        }

        else if (command == 15) {
            // складывание
            int used_servo[4] = { 0 };
            int servo_signal[4] = { 0 };
            for (int i = 0; i < 4; i++) {
                used_servo[i] = ((servo_data & (1 << i)) >> i);
                // servo_signal[i] = servo_payload[i] & ( (used_servo[i] & servo_states[i]) | (!used_servo[i]) );
                if (servo_payload[i] == 1) {
                    if (used_servo[i] == 1) {
                        servo_signal[i] = servo_states[i];
                        if (servo_states[i] == 0) {
                            servo_payload[i] = 0;
                        }
                    }
                    else {
                        servo_signal[i] = 1;
                    }
                }
                else {
                    servo_signal[i] = 0;
                }
            }

            sms_sts.WritePosEx(5, (1250 + 1900) / 2, 3400, 50);
            go_down();
            osDelay(1700);

            set_grippers(servo_signal);
            osDelay(700);
            set_height(800);
            osDelay(1200);

            for (int i = 0; i < 4; i++) {
                // servo_signal[i] = servo_payload[i] & ( (used_servo[i] & (1 - servo_states[i])) | (!used_servo[i]) );
                if (servo_payload[i] == 1) {
                    if (used_servo[i] == 1) {
                        servo_signal[i] = 1 - servo_states[i];
                        if (servo_states[i] == 1) {
                            servo_payload[i] = 0;
                        }
                    }
                    else {
                        servo_signal[i] = 1;
                    }
                }
                else {
                    servo_signal[i] = 0;
                }
                // servo_payload[i] = servo_payload[i] & (1 - used_servo[i]);
            }

            set_grippers(servo_signal);
        }
        else if (command == 1) { // 16
            sms_sts.CalibrationOfs(6);
            osDelay(1000);
            sms_sts.CalibrationOfs(7);
            osDelay(1000);
        }
        else if (command == 2) { // 32
            set_height(-700);
        }
        else if (command == 3) { // 48
            set_grippers(servo_open_states);
            sms_sts.WritePosEx(5, 1900, 1500, 50);
            osDelay(500);
            set_height(500);
        }
        else if (command == 4) { // 64
            set_height(800);
        }
        else if (command == 7) { // 112
            // скольжение 1
            int servo_signal[4] = { 0 };
            bool found_cand = false;
            for (int i = 0; i < 4; i++) {
                if ((servo_payload[i] == 1) && (!found_cand) && (servo_states[i] == 1)) {
                    servo_signal[i] = 0;
                    servo_payload[i] = 0;
                    found_cand = true;
                }
                else if (servo_payload[i] == 1) {
                    servo_signal[i] = 1;
                }
                else {
                    servo_signal[i] = 0;
                }
            }
            sms_sts.WritePosEx(5, (1250 + 1900) / 2, 3400, 50);
            set_height(800);
            osDelay(800);
            set_grippers(servo_signal);

        }
        else if (command == 9) { // 144
            // складывание 1
            int servo_signal[4] = { 0 };
            bool found_cand = false;
            for (int i = 0; i < 4; i++) {
                if ((servo_payload[i] == 1) && (!found_cand) && (servo_states[i] == 0)) {
                    servo_signal[i] = 0;
                    servo_payload[i] = 0;
                    found_cand = true;
                }
                else if (servo_payload[i] == 1) {
                    servo_signal[i] = 1;
                }
                else {
                    servo_signal[i] = 0;
                }
            }
            sms_sts.WritePosEx(5, (1250 + 1900) / 2, 3400, 50);
            go_down();
            osDelay(1700);
            set_grippers(servo_signal);
            osDelay(700);
            set_height(800);
        }
                else if (command == 5) { // 80
                    // скольжение 2
                    int servo_signal[4] = { 0 };
                    for (int i = 0; i < 4; i++) {
                        if (servo_payload[i] == 1) {
                            servo_signal[i] = 1 - servo_states[i];
                            if (servo_states[i] == 1) {
                                servo_payload[i] = 0;
                            }
                        }
                        else {
                            servo_signal[i] = 0;
                        }
                    }
                    sms_sts.WritePosEx(5, (1250 + 1900) / 2, 3400, 50);
                    set_height(800);
                    osDelay(800);
                    set_grippers(servo_signal);

                }
                else if (command == 6) { // 96
                    // складывание 2
                    int servo_signal[4] = { 0 };
                    for (int i = 0; i < 4; i++) {
                        if (servo_payload[i] == 1) {
                            servo_signal[i] = servo_states[i];
                            if (servo_states[i] == 0) {
                                servo_payload[i] = 0;
                            }
                        }
                        else {
                            servo_signal[i] = 0;
                        }
                    }
                    sms_sts.WritePosEx(5, (1250 + 1900) / 2, 3400, 50);
                    go_down();
                    osDelay(1700);
                    set_grippers(servo_signal);
                    osDelay(700);
                    set_height(800);
                }
        else if (command == 8) { // 128
            sms_sts.WritePosEx(5, (1250 + 1900) / 2, 3400, 50);
            go_down();
            osDelay(1500);
            set_grippers(servo_open_states);
            osDelay(700);
            set_height(500);
        }


        servo_spin_flag = false;
    }
}

//#include "ServoTask.h"
//#include "SCServo.h"
//#include "usart.h"
//#include "cmsis_os.h"
//
//extern uint8_t servo_data;
//extern bool servo_spin_flag;
//
//HardwareSerial SerialServo(UART4, 1000000); // 115200
//SMS_STS sms_sts;
//
//int servo_close_states[4] = { 1, 1, 1, 1 };
//int servo_open_states[4] = { 0, 0, 0, 0 };
//
//int servo_states[4] = { 0 };
//int servo_payload[4] = { 0, 0, 0, 0 };
//
//static void set_grippers(int* states) {
//    u8 servos_ids[] = { 1, 2, 3, 4 };
//    u16 speed[] = { 3400, 3400, 3400, 3400 };
//    u8 acc[] = { 50, 50, 50, 50 };
//    s16 position[4];
//
//    for (int i = 0; i < 4; i++) {
//        if (states[i])
//            position[i] = 880;
//        else
//            position[i] = 2048;
//    }
//
//    sms_sts.SyncWritePosEx(servos_ids, 4, position, speed, acc);
//}
//
//static void set_height(s16 pos) {
//    u8 servos_ids[] = { 6, 7 };
//    u16 speed[] = { 3400, 3400 };
//    u8 acc[] = { 25, 25 };
//    s16 position[2];
//
//    position[0] = 2047 + pos;
//    position[1] = 2047 - pos;
//    sms_sts.SyncWritePosEx(servos_ids, 2, position, speed, acc);
//}
//
//static void go_down() {
//    int pos_down = -1550;
//    set_height(pos_down);
//}
//
//void ServoInit() {
//    SerialServo.begin();
//    sms_sts.pSerial = &SerialServo;
//}
//
//void ServoSpin() {
//    uint8_t command = (servo_data & 0xF0) >> 4;
//
//    if (servo_spin_flag == true) {
//
//        if (command == 0) {
//            // взятие
//            set_grippers(servo_open_states);
//            sms_sts.WritePosEx(5, 1250, 3400, 50); // 1900 - mid, 1250 - open
//            osDelay(300);
//            go_down();
//            osDelay(1300);
//            sms_sts.WritePosEx(5, 1900, 3400, 50);
//
//            osDelay(300);
//            set_grippers(servo_close_states);
//            osDelay(800);
//            set_height(500);
//
//            for (int i = 0; i < 4; i++) {
//                servo_states[i] = ((servo_data & (1 << i)) >> i);
//                servo_payload[i] = 1;
//            }
//        }
//
//        else if (command == 15) {
//            // складывание
//            int used_servo[4] = { 0 };
//            int servo_signal[4] = { 0 };
//            for (int i = 0; i < 4; i++) {
//                used_servo[i] = ((servo_data & (1 << i)) >> i);
//                // servo_signal[i] = servo_payload[i] & ( (used_servo[i] & servo_states[i]) | (!used_servo[i]) );
//                if (servo_payload[i] == 1) {
//                    if (used_servo[i] == 1) {
//                        servo_signal[i] = servo_states[i];
//                        if (servo_states[i] == 0) {
//                            servo_payload[i] = 0;
//                        }
//                    }
//                    else {
//                        servo_signal[i] = 1;
//                    }
//                }
//                else {
//                    servo_signal[i] = 0;
//                }
//            }
//
//            sms_sts.WritePosEx(5, (1250 + 1900) / 2, 3400, 50);
//            go_down();
//            osDelay(1700);
//
//            set_grippers(servo_signal);
//            osDelay(700);
//            set_height(800);
//            osDelay(1200);
//
//            for (int i = 0; i < 4; i++) {
//                // servo_signal[i] = servo_payload[i] & ( (used_servo[i] & (1 - servo_states[i])) | (!used_servo[i]) );
//                if (servo_payload[i] == 1) {
//                    if (used_servo[i] == 1) {
//                        servo_signal[i] = 1 - servo_states[i];
//                        if (servo_states[i] == 1) {
//                            servo_payload[i] = 0;
//                        }
//                    }
//                    else {
//                        servo_signal[i] = 1;
//                    }
//                }
//                else {
//                    servo_signal[i] = 0;
//                }
//                // servo_payload[i] = servo_payload[i] & (1 - used_servo[i]);
//            }
//
//            set_grippers(servo_signal);
//        }
//        else if (command == 1) { // 16
//            sms_sts.CalibrationOfs(6);
//            osDelay(1000);
//            sms_sts.CalibrationOfs(7);
//            osDelay(1000);
//        }
//        else if (command == 2) { // 32
//            set_height(-700);
//        }
//        else if (command == 3) { // 48
//            set_grippers(servo_open_states);
//            sms_sts.WritePosEx(5, 1900, 1500, 50);
//            osDelay(500);
//            set_height(500);
//        }
//        else if (command == 4) { // 64
//            set_height(800);
//        }
//        else if (command == 5) { // 80
//            // скольжение 2
//            int servo_signal[4] = { 0 };
//            for (int i = 0; i < 4; i++) {
//                if (servo_payload[i] == 1) {
//                    servo_signal[i] = 1 - servo_states[i];
//                    if (servo_states[i] == 1) {
//                        servo_payload[i] = 0;
//                    }
//                }
//                else {
//                    servo_signal[i] = 0;
//                }
//            }
//            sms_sts.WritePosEx(5, (1250 + 1900) / 2, 3400, 50);
//            set_height(800);
//            osDelay(800);
//            set_grippers(servo_signal);
//
//        }
//        else if (command == 6) { // 96
//            // складывание 2
//            int servo_signal[4] = { 0 };
//            for (int i = 0; i < 4; i++) {
//                if (servo_payload[i] == 1) {
//                    servo_signal[i] = servo_states[i];
//                    if (servo_states[i] == 0) {
//                        servo_payload[i] = 0;
//                    }
//                }
//                else {
//                    servo_signal[i] = 0;
//                }
//            }
//            sms_sts.WritePosEx(5, (1250 + 1900) / 2, 3400, 50);
//            go_down();
//            osDelay(1700);
//            set_grippers(servo_signal);
//            osDelay(700);
//            set_height(800);
//        }
//        else if (command == 8) { // 128
//            sms_sts.WritePosEx(5, (1250 + 1900) / 2, 3400, 50);
//            go_down();
//            osDelay(1500);
//            set_grippers(servo_open_states);
//            osDelay(700);
//            set_height(500);
//        }
//
//
//        servo_spin_flag = false;
//    }
//}
//
