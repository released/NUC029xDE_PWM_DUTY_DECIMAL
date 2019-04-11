# NUC029xDE_PWM_DUTY_DECIMAL

update @ 2019/04/11

Add LED dimming with PWM function (PA12)

change PWM duty per 500ms , calculate CNR , CMR by PWM CLOCK , FREQ , 

target Duty (extend to decimal , ex : 10.3% = 103 , 25.9% = 259)

with below formula : 

CNR = PWM_CLOCK/(PWM_PSC+1)/PWM_FREQ - 1

CMR = (CNR+1)*(PWM_DUTY_1000/1000)-1
