#include "cmd_proc.h"
#include "slt_control.h"

static void handle_param_config(uint8_t* cmd);
static int parse_param_config_command(uint8_t* cmd, ParamConfigParams_t* params);
static int process_param_config_boards(ParamConfigParams_t* params);


int pc_process_received_command(uint8_t* cmd, uint32_t len)
{
    // check the params
	if (cmd == NULL || *cmd == NULL) {
        return -1;
    }
    if (len == NULL) {
        return -1;
    }

	// Remove trailing newline characters if present
    if (len > 0 && cmd[len - 1] == '\n') {
        cmd[len - 1] = '\0';
        len--;
    }
    if (len > 0 && cmd[len - 1] == '\r') {
        cmd[len - 1] = '\0';
        len--;
    }

	// Check if command is empty after trimming
    if (len == 0)return -1;

    // Command routing
	if(strstr((char*)cmd, CMD_PARAM_CONFIG) != NULL){
		handle_param_config(cmd);
	}
    else if (strstr((char*)cmd, CMD_POWER_ON) != NULL) {
        GPIO_SetBits(VCC_PORT,VCC_PIN);
		Usart_SendString(PC_USART,RESULT_POWER_ON_PASS);
    }
    else if (strstr((char*)cmd, CMD_POWER_DOWN) != NULL) {
        GPIO_ResetBits(VCC_PORT,VCC_PIN);
		Usart_SendString(PC_USART,RESULT_POWER_DOWN_PASS);
    }

	return 0;
}

static void handle_param_config(uint8_t* cmd)
{
    ParamConfigParams_t params = {0};

    // Parse command string and extract parameters
    if(parse_param_config_command(cmd, &params) != 0) 
	{return;}

	process_param_config_boards(&params);

    Usart_SendString(PC_USART,RESULT_PARAM_CONFIG_PASS);

}

static int parse_param_config_command(uint8_t* cmd, ParamConfigParams_t* params)
{
    char* token;
    char* saveptr;
    int param_count;
	char cmd_copy[128] = {0};
	strncpy(cmd_copy, (char*)cmd, sizeof(cmd_copy) - 1);
	cmd_copy[sizeof(cmd_copy) - 1] = '\0';

    memset(params, 0, sizeof(ParamConfigParams_t));

    // skip the command "@SParam"
    token = strtok_r(cmd_copy, "#", &saveptr);
    if(token == NULL)
	{return -1;}

    // the number of para
    token = strtok_r(NULL, "#", &saveptr);
    if(token == NULL) 
	{return -1;}
    param_count = atoi(token);

    //check the count para
    if(param_count <= 0) 
	{return -1;}

    // para1 gpio[0:9]
    token = strtok_r(NULL, "#", &saveptr);
	if(token) params->gpio_status = (uint16_t)strtoul(token, NULL, 2);

    // para2 test checksum
    token = strtok_r(NULL, "#", &saveptr);
    if(token) params->checksum = (uint32_t)strtoul(token, NULL, 10);

    return 0;
}

static int process_param_config_boards(ParamConfigParams_t* params)
{
	uint8_t ones_count = 0;
	
	if (params == NULL) {
        return -2;
    }
	
	for (uint8_t i = 0; i < 10; i++) {
        if ((params->gpio_status >> i) & 0x01) {
            ones_count++;
        }
    }
	
	if (ones_count != params->checksum) {
        return -1; 
    }
	
	for (int8_t i = 9; i >= 0; i--) {
        if ((params->gpio_status >> i) & 0x01){
            Set_SLT_GPIO(i);
        }else{
            Reset_SLT_GPIO(i);
        }
    }
	
	return 0;
}


int slt_process_received_command(uint8_t* cmd, uint32_t len)
{
	 char* start;
    char* end;
    char* p;
    uint32_t cmd_len;
    uint32_t remaining;
    int found_count = 0;
    
    if (cmd == NULL || len == 0) {
        return -1;
    }
    
    if (len < SLT_UART_BUF_MAX_LEN) {
        cmd[len] = '\0';
    }

    p = (char*)cmd;
    remaining = len;
    
    while (remaining > 2 && (start = strstr(p, "@S")) != NULL) {
        remaining = len - (start - (char*)cmd);
        
        if (remaining < 6) {
            break;
        }
        
        end = strstr(start, "#E@");
        if (end == NULL) {
            break;
        }
        
        cmd_len = (end - start) + 3; 
        
        if (cmd_len > remaining) {
            break;
        }
        
        char saved_char = '\0';
        if (end[3] != '\0') {
            saved_char = end[3];
        }
        
        end[3] = '\0';
        
        Usart_SendString(PC_USART, start);
        found_count++;
        
        if (saved_char != '\0') {
            end[3] = saved_char;
        }
        
        p = end + 3;
        
        remaining = len - (p - (char*)cmd);
        
        if (remaining < 6) {
            break;
        }
    }
    
    return found_count;
}
