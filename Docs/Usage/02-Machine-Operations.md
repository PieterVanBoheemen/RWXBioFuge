02 Machine Operations
=====================

The RWXBioFuge is not a difficult machine to operate. Still, it's wise to familiarize yourself with the user interface before conducting any experiments.

The user interface consists of:
- Power Switch
- 3 buttons
  - Start
  - Stop
  - Short
- 2 knobs
  - Time
  - Speed

### Normal user flow:
1. Start the machine by flipping the power switch, the machine will start up in Configuration Mode
2. Load the balanced samples into the rotor and close the lid
3. Turn the Time and Speed knob to set the desired length of time and percentage of power
4. Press the Start button
  - The LCD screen will show a count down and indicate then the spin is done
  - You may press the Stop button at any time to interrupt the current process
5. The machine returns to the start screen Configuration Mode
6. If you do not plan to use the machine any more, switch it off using the power switch

Flow diagram:
```flow
st=>start: Begin
io1=>inputoutput: Set Speed
io2=>inputoutput: Set Time
op1=>operation: Press Start
op2=>operation: Spin Rotor
cond=>condition: Stop Pressed
e=>end

st->io1->io2->op1->op2->cond
cond(no)->e
cond(yes)->op2
```

### "Short" user flow:
1. Start the machine by flipping the power switch, the machine will start up in Configuration Mode.
2. Load the balanced samples into the rotor and close the lid.
3. Press the Short button.
4. The rotor will start spinning immediately.
5. Release the Short button to stop the rotor.
6. The machine returns to the start screen Configuration Mode
7. If you do not plan to use the machine any more, switch it off using the power switch

Flow diagram:
```flow
st=>start: Begin
op1=>operation: Press Short
op2=>operation: Spin Rotor
cond=>condition: Short released
e=>end

st->op1->op2->cond
cond(no)->op2
cond(yes)->e
```
