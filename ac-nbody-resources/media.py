import os

folder = "./resultados"

def processa_lista(lst):
    return [ float(a)*60+float(b) for a,b in [ x.split("\t")[1].replace("s","").split("m") for x in lst ]]


def get_medias(arquivo_name):
    with open(f"{folder}/{arquivo_name}","r") as arquivo:
	    my_list = arquivo.read().split("\n")
	    listas = { z: processa_lista(list(filter(lambda x: z in x, my_list))) for z in ["real","sys","user"] }
	    listas = { x : sum(listas[x])/len(listas[x]) for x in listas.keys() }
	    return listas
	

for arquivo_name in os.listdir(folder):
    if not "time" in arquivo_name: continue
    print(arquivo_name)
    print(get_medias(arquivo_name))
    # try: print(get_medias(arquivo_name))
    # except: print("checar arquivo")
    print("="*60)
