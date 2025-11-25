import interface
import gui
import sampler
import player

interface_i = interface.Interface(550)
sampler_i = sampler.Sampler()
sampler_i.start_microphone_sampling()
player_i = player.Player(interface_i)
gui_i = gui.Gui(interface_i, sampler_i, player_i)
