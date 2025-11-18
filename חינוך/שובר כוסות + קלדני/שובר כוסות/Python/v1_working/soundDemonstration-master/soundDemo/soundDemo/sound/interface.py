'''
' Created: May 12, 2015
'
' @author: Omer Cohen
'''

LOW_VOL = 0.05
HIGH_VOL = 0.5

class Interface():

    def __init__(self, frequency):
        self.vol = LOW_VOL
        self.freq = frequency
        self.finetune = 0.1
        self.coarsetune = 1

    def mute(self):
        self.vol = 0

    def increaseVol(self):
        self.vol += 1

    def decreaseVol(self):
        self.vol -=1
        
    def setVol(self, vol):
        if vol < 0:
            self.vol = 0 
        elif vol > 1:
            self.vol = 1
        else:
            self.vol = vol
        
    def whichVol(self):
        if self.vol == LOW_VOL:
            return 'low'
        else:
            return 'high'

    def setTuneStep(self, val, fine=0):
        if fine:
            self.finetune = val
        else:
            self.coarsetune = val
            
    def increaseFreq(self):
        self.freq += self.coarsetune

    def increaseFreqFine(self):
        self.freq += self.finetune

    def decreaseFreq(self):
        self.freq -= self.coarsetune

    def decreaseFreqFine(self):
        self.freq -= self.finetune
        
    def setFreq(self, freq):
        self.freq = freq
        if freq < 0.1:
            self.freq = 0.1

