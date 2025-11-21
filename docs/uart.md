- On the development board F446RE, there are multiple uart
- There's USUART2 that's connect to the ST-LINK2 on PA2 and PA3 according to the schematics and documentations
- According to the manual, we can set the GPIO to different mode:
    - Input floating
    - Input pull-up
    - Input pull-down
    - Analog
    - Output open-drain with pull-up or pull-down capability
    - Output push-pull with pull-up or pull-down capability
    - Alternate function push-pull with pull-up or pull-down capability
    - Alternate function open-drain with pull-up or pull-down capability

 The below images is take from pages of reference manual
---
![How to set port](image.png)
---

- After switching the mode to the alternate function, we have to enable the specific peripheral function that is mapped to a specific AF (Alternate function) number. For PA2, the alternate function to use USART2 is AF7. More about alternate function can be learn from [this link](https://www.youtube.com/watch?v=1841Gvf0Gpc)

Pages 58 of the datasheet
---
![alt text](image-1.png)
---
Pages 179 of the reference manual
---
![alt text](image-2.png)
---