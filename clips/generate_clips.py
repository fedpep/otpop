from PIL import Image
import sys
from clip_indexes import *

HEADER="/* Don't modify this file. It has been autogenerated with the %s script */\n"%"generate_clips.py"
STATE_SEARCH_Y,STATE_SEARCH_X,STATE_SEARCH_H,STATE_SEARCH_W=range(4)

class Rect(object):
    def __init__(self, x, y, w ,h):
        self.x=x
        self.y=y
        self.w=w
        self.h=h

    def __str__(self):
        return ("{(%d,%d)  %d,%d}"%(self.x,self.y,self.w,self.h))

file_name_input=sys.argv[1]
file_name=sys.argv[2]

im = Image.open("%s.bmp"%file_name_input)
pix = im.load()

def next_pixel_row(x,y):
    x=x+1
    if x==im.size[0]:
        x=0
        y=y+1
        if y==im.size[1]:
            y=reset_y
            #x=0
    return (x,y)

def next_pixel_col(x,y):
    y=y+1
    if y==im.size[1]:
        y=reset_y
        x=x+1
        if x==im.size[0]:
            x=0
            #return (0,0)
    return (x,y)

def set_state(new_state):
    global state
    state=new_state
    #print " changing state: %d"%state

c_y=0
c_h=0
clips=[]
for k in range(15):
    #print "===================%d================="%k
    x=0
    reset_y=c_y+c_h
    y=reset_y
    c_x=im.size[0]
    c_h=0
    clips+=[[]]
    set_state(STATE_SEARCH_Y)
    while 1:
        if state==STATE_SEARCH_Y:
            if pix[x,y]!=(255,255,255):
                c_y=y
                set_state(STATE_SEARCH_X)
                x=0
            else:
                (x,y)=next_pixel_row(x,y)
                #if (x,y)==(0,0): break

        elif state==STATE_SEARCH_X:
            #print x,y
            if pix[x,y]!=(255,255,255):

                if x<c_x:
                    c_x=x
                    #print "min",c_x
                if len(clips[-1]):
                    x=clips[-1][-1].x+clips[-1][-1].w+1
                else:
                    x=0
                y=y+1
            else:
                (x,y)=next_pixel_row(x,y)
            if (x,y)==(0,0): break


            if x==im.size[0]-1:
                #x=c_x
                if c_x==im.size[0]:
                    if len(clips[-1]):
                        x=clips[-1][-1].x+clips[-1][-1].w+1
                    else:
                        x=0
                    y=y+1
                    if y==c_y+c_h:
                        break
                else:    
                    if c_h:
                        set_state(STATE_SEARCH_W)
                    else:
                        set_state(STATE_SEARCH_H)
                    x=c_x
                    y=c_y
                
        elif state==STATE_SEARCH_H:
            if pix[x,y]!=(255,255,255):
                x=c_x
                y=y+1
                #print 'reset'
            else:
                (x,y)=next_pixel_row(x,y)

            if x==im.size[0]-1:
                c_h=y-c_y
                x=c_x
                y=c_y
                set_state(STATE_SEARCH_W)
                
        elif state==STATE_SEARCH_W:
            #print x,y
            if pix[x,y]!=(255,255,255):
                x=x+1
                y=c_y
            else:
                (x,y)=next_pixel_col(x,y)

            if y==c_y+c_h:
                c_w=x-c_x
                clips[-1]+=[Rect(c_x,c_y,c_w,c_h)]
                #print clips[-1]
                x=c_x+c_w+1

                c_x=im.size[0]
                y=c_y
                if c_y:
                    set_state(STATE_SEARCH_X)
                else:
                    set_state(STATE_SEARCH_Y)
                
    #print "%d)"%k, ','.join("%s"%c for c in clips[-1])
        


im2=Image.new(im.mode,(im.size[0]*2,im.size[1]),(255,255,255))
for i in range(len(clips)):
    for j in range(len(clips[i])):
        box=[clips[i][j].x,clips[i][j].y,clips[i][j].x+clips[i][j].w,clips[i][j].y+clips[i][j].h]
        region=im.crop(tuple(box))
        im2.paste(region,tuple(box))
        region=region.transpose(Image.FLIP_LEFT_RIGHT)
        box[0]+=im.size[0]
        box[2]+=im.size[0]
        im2.paste(region,tuple(box))
im2.save('%s.bmp'%file_name,'BMP')


for i in range(len(clips)):
    for j in range(len(clips[i])):
        clips[i]+=[Rect(clips[i][j].x+im.size[0],clips[i][j].y,clips[i][j].w,clips[i][j].h)]





content=HEADER
content+="""#include <SDL.h>
#include "%s.h"
"""%file_name

n=0
for i in range(len(clips)):
    content+="#define CLIP_ROW_%d    (%d)\n"%(i,n)
    content+="#define CLIP_ROW_%d_H    (%d)\n"%(i,n+len(clips[i])/2)
    n+=len(clips[i])


content+="#define %s_SIZE   (%d)\n"%(file_name.upper(),n)

content+="""
static SDL_Rect clips[]={
"""

n=0
for i in range(len(clips)):
    #print i,' - '
    for c in clips[i]:
        content+="     {.x=%d,.y=%d,.w=%d,.h=%d},\n"%(c.x,c.y,c.w,c.h)
        n+=1
        #print ' ',c
content+="""};

"""
f=open('../types.h','r')
types_cont=f.read()
f.close()

type_decl=types_cont.find('chr_state_t;')

for i in range(type_decl,-1,-1):
    if types_cont[i]=='}':
        end_idx=i
    elif types_cont[i]=='{':
        start_idx=i
        break

states_list=(types_cont[start_idx:end_idx+1].replace('{','').replace('}','').replace('\n','').replace(' ','').split(','))[:-2]

#print states_list

content+="""
static int state_clips[][4]={
"""


for i in range(len(states_list)):    
    try:
        ci=CLIPS_INDEXES[states_list[i]]
        content+="         {%s+%d,%d,%d,%d},\n"%(ci[0],ci[1],ci[2],ci[3],ci[4])
        
    except KeyError:
        print "key error!"

for i in range(len(states_list)):    
    try:
        ci=CLIPS_INDEXES[states_list[i]]
        content+="         {%s_H+%d,%d,%d,%d},\n"%(ci[0],ci[1],ci[2],ci[3],ci[4])

    except KeyError:
        print "key error!"

content+="""
};
"""
content+="""
void %s_init(figure_t *f)
{
      f->clips=%s;
      f->clips_size=%s_SIZE;
      f->get_clip_indexes=%s_get_index;
}
"""%(file_name,'clips',file_name.upper(),file_name)

content+="""
void %s_get_index(chr_state_t s, uint8_t direction, int **ind)
{
    uint8_t i=s;
    
    if(direction)
       i+=%d;

    (*ind)=state_clips[i];
}

"""%(file_name,len(states_list))

f=open("%s.c"%file_name,"w")
f.write(content)
f.close()

content=HEADER
content+='#include "types.h"\n'


content+="""
void %s_init(figure_t *f);
void %s_get_index(chr_state_t s, uint8_t direction, int **ind);

"""%(file_name,file_name)

f=open("%s.h"%file_name,"w")
f.write(content)
f.close()
