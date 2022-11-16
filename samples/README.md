# Dataset of samples for the MSP training
Each folder stores sample of the declared label

# *Guidelines for dataset generation*
Every line must hold the same number of values [256]
Every item is the single vector is comma separated
You can use # to introduce a line of comment

# Guideline to understand directories
0: PLAY,
1: STOP,

# XXX-aug directories
The above mentioned directories will store the augmented datasets under the following protocol.
At random once frequency will be init to a random value between 20Hz to 20kHz to introduce some noise in the data.