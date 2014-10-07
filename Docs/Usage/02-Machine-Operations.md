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

Normal user flow:
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

"Short" user flow:
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

```flow
st=>start: Start
e=>end
op=>operation: My Operation
cond=>condition: Yes or No?

st->op->cond
cond(yes)->e
cond(no)->op
```
