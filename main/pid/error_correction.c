// #include <stdio.h>

// struct pid_params_t {
//     float kp;
//     float ki;
//     float kd;

//     float epsilon;
//     float ratio_to_bearing;
//     float setpoint;
//     float integral;
//     float prev_error;
//     float current_ratio;
//     float current_bearing;
// } pid_params;

// void init_pid(){
//     pid_params.kp = 0.01f;
//     pid_params.ki = 0.05f;
//     pid_params.kd = 0.01f;

//     pid_params.epsilon = 0.01f;
//     pid_params.ratio_to_bearing = -0.007f;
//     pid_params.setpoint = 0.0f;
//     pid_params.integral = 0.0f;
//     pid_params.prev_error = 0.0f;
//     pid_params.current_ratio = 0.0f;
//     pid_params.current_bearing = 0.0f;
// }

// float calculate_pid(float current_bearing, float target_bearing, float current_ratio){
//     float error = (float)((int)(target_bearing - current_bearing + 180) % 360 - 180);

//     pid_params.integral += error;
//     float derivative = error - pid_params.prev_error;

//     float control_signal = pid_params.kp * error + pid_params.ki * pid_params.integral + pid_params.kd * derivative;

//     pid_params.prev_error = error;

//     return control_signal;
// }

// void bearing_correction(float target_bearing){ 
//     while((int)pid_params.current_bearing != (int)target_bearing){
//         float control_signal = calculate_pid(pid_params.current_bearing, target_bearing, pid_params.current_ratio);

//         float new_ratio = (pid_params.current_ratio + pid_params.epsilon) + ((control_signal + pid_params.epsilon + 1) * pid_params.ratio_to_bearing);

//         pid_params.current_bearing += (new_ratio - pid_params.current_ratio) / pid_params.ratio_to_bearing;
//         pid_params.current_ratio = new_ratio;

//         printf("Current bearing: %f\t", pid_params.current_bearing);
//         printf("Current ratio: %f\n", pid_params.current_ratio);

//         Sleep(100);
//     }   
//     printf("Complete!\n");
// }

// int main(){

//     init_pid();

//     pid_params.current_ratio = 1.25f;
//     pid_params.current_bearing = 320.0f;

//     float target_bearing = 290.0f;
//     bearing_correction(target_bearing);

//     return 0;
// }
