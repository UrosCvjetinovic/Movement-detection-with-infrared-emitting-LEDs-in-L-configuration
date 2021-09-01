#ifndef __PROXIMITY_API_H_
#define __PROXIMITY_API_H_

extern uint32_t GetSpecs(void);
extern void Start(void);
extern void StartMessuring(void);
extern void Setup(void);
extern uint16_t ReadOutput(uint8_t ui8Addr);

#endif /* __PROXIMITY_API_H_ */
