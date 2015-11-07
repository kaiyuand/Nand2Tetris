import sys
import os

#define all the legal keywords,symbols and numbers in jack.

jackdic =   {   'keywords':['class', 
                            'constructor','function','method',
                            'field','static','var',
                            'int','char', 'boolean', 'void', 
                            'true','false','null','this',
                            'let', 'do', 'if','else','while','return'
                            ], 
                'symbols': ['{','}','(',')','[',']',
                            '.',',',';',
                            '+', '-','*','/','&','|',
                            '<', '>', '=', '~'
                            ], 
                'numbers': ['0','1','2','3','4','5','6','7','8','9']
            }

def begin(fullname):
    jacktext = deletecomments(open(fullname))
    jacktext = transfer(jacktext)
    return jacktext

def deletecomments(filename):
    jacktext = []
    ignore_on = False
    for line in filename:
        if '//' in line:
            line = line[:line.index('//')]
        for chari in range(len(line)):
            if line[chari] == '/' and chari != len(line)-1:
                if line[chari + 1] == '*':
                    ignore_on = True
            if '\\' in ('%r'%line[chari]):
                jacktext.append(' ')
                continue
            if ignore_on:
                continue
            jacktext.append(line[chari])
        if ignore_on == True and '*/' in line:
            line = line[line.index('*/') + 2:]
            ignore_on = False
    jacktext = ''.join(jacktext)
    return jacktext 

def transfer(jacktext):
    global jackdic
    begin = ["<tokens>"]
    index = 0
    token = []
    while index < len(jacktext):
        token = []
        #jump the space
        if jacktext[index] == ' ':
            index += 1
        # transfer the symbols
        # three different ones '<','>','&'
        elif jacktext[index] in jackdic['symbols']:
            if jacktext[index] == '<':
                sym = '&lt;'
            elif jacktext[index] == '>':
                sym = '&gt;'
            elif jacktext[index] == '&':
                sym = '&amp;'
        # other symbols
            else:
                sym = jacktext[index]
            begin.append("<symbol> %s </symbol>" % sym)
            index += 1
        #constant numbers ->integerconstant
        elif jacktext[index] in jackdic['numbers']:
            while jacktext[index] in jackdic['numbers']:
                token.append(jacktext[index])
                index += 1
            begin.append("<integerConstant> %s </integerConstant>" % 
                           ''.join(token)) 
        #string constant between ""     
        elif jacktext[index] == '"':
            index += 1
            while jacktext[index] != '"':
                token.append(jacktext[index])
                index += 1
            begin.append("<stringConstant> %s </stringConstant>" % 
                           ''.join(token))
            index += 1
        # keywords and identifiers
        else:
            token_found = False
            while token_found == False:
                token.append(jacktext[index])
                index += 1
                if jacktext[index] in jackdic['symbols'] or jacktext[index] in ['"', ' ']:
                    if ''.join(token) in jackdic['keywords']:
                        begin.append("<keyword> %s </keyword>" % 
                                       ''.join(token))
                        token_found = True
                    else: 
                        begin.append("<identifier> %s </identifier>" % 
                                       ''.join(token))
                        token_found = True
    begin.append("</tokens>")
    return begin





