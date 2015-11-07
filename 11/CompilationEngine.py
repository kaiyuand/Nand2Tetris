import sys
import os
import JackTokenizer
import pdb

# process the single file one by one
def process_file(filename,pathname=''):
    fullname = []
    if pathname == '':
        x = ''
    else:
        x = '/'
    print "Compiling file: %s" % (pathname + x + filename)
    fullname = pathname + x + filename
    tokenized_file = JackTokenizer.begin(fullname)
    tokenized_text = Vars(tokenized_file)
    parser(tokenized_text, filename[:-5], pathname)
    print "Waiting for ..."
    print "Compilation Completed!"
    return
# get all the jack files in the directory
def process_directory(pathname):
    for filename in os.listdir('%s' % pathname):
        if filename[-5:] == '.jack':
            process_file(filename, pathname)

class singletoken():

    def __init__(self, line, index, text):
        self.kind = self._getType(line)
        self.name = self._getName(line)
        self.index = index
        self.line = line
        self.text = text

    def run(self):
        return self.text.tokens[self.index + 1]

    def back(self):
        return self.text.tokens[self.index - 1]

    def next(self):
        self.text.cursor += 1
        return self.text.tokens[self.text.cursor]

    def reset(self):
        return self.text.tokens[self.text.cursor]

    def _getType(self, line):
        return line[line.find('<')+1:line.find('>')]

    def _getName(self, line):
        return line[line.find('>')+2:line.find('<',2)-1]

class Vars:

    def __init__(self, filename):
        self.tokens = []
        self.cursor = 0
        self.Class = {}
        self.Subroutine = {}

        i = 0
        for line in filename:
            if line[-1] != '\n':
                self.tokens.append(singletoken(line + '\n', i, self))
            else:
                self.tokens.append(singletoken(line, i, self))
            i += 1
        return

    def init(self):
        self.Subroutine = {}
        return

    def variables(self, kind):
        assert kind in ['static', 'field', 'arg', 'var']
        # global variable
        if kind in ['static', 'field']:
            return sum(x[0] == kind for x in self.Class.values())
        # internal variable
        else:
            return sum(x[0] == kind for x in self.Subroutine.values())

    def tokenK(self, name):
        if name in self.Subroutine.keys():
            return self.Subroutine[name][0]
        elif name in self.Class.keys():
            return self.Class[name][0]
        else:
            return None

    def tokenT(self, name):
        if name in self.Subroutine.keys():
            return self.Subroutine[name][1]
        elif name in self.Class.keys():
            return self.Class[name][1]
        else:
            return None

    def TokenI(self, name):
        if name in self.Subroutine.keys():
            return self.Subroutine[name][2]
        elif name in self.Class.keys():
            return self.Class[name][2]
        else:
            return None

    def tokenbegin(self, name, types, kind):
        assert kind in ['static', 'field', 'arg', 'var']
        index = self.variables(kind)
        if kind in ['static', 'field']:
            self.Class[name] = (kind, types, index)
        else:
            self.Subroutine[name] = (kind, types, index)
        return



def parser(text, filename, directory=''):
    newtext = []
    segmentdict = {'const':'constant', 'arg':'argument', 'local':'local',
                   'var':'local', 'static':'static', 'field':'this', 
                   'this':'this', 'that':'that', 'pointer':'pointer', 
                   'temp':'temp'}
    segments = ['constant', 'argument', 'local', 'static', 'this', 'that', 
                   'pointer', 'temp']
    commands = ['add', 'sub', 'neg', 'eq', 'gt', 'lt', 'and', 'or', 'not']
    ops = {'+':'add', '-':'sub', '*':None, '/':None, '&amp;':'and', '|':'or',              '&lt;':'lt', '&gt;':'gt', '=':'eq'}
    unaryops = {'-':'neg', '~':'not'}
    keywords = ['true', 'false', 'null', 'this']
    ind = '   '
    output = None

    def VMwriter(classname, directory):
        if directory !='':
            parser.output = open(directory + '/' + classname + '.vm', 'w')
        else:
            parser.output = open(classname + '.vm', 'w')

    def writePush(segment, index=''):
        assert segment in segments
        if index != '':
            assert type(index) == int
        parser.output.write(ind + "push " + segment + " " + 
                                     str(index) + '\n')

    def writePop(segment, index=''):
        assert segment in segments
        if index != '':
            assert type(index) == int
        parser.output.write(ind + "pop " + segment + " " + 
                                     str(index) + '\n')

    def writeArithmetic(command):
        assert command in commands
        parser.output.write(ind + command + '\n')

    def writeLabel(label):
        assert type(label) == str
        parser.output.write("label " + label + '\n')

    def writeGoto(label):
        assert type(label) == str
        parser.output.write(ind + "goto " + label + '\n')

    def writeIf(label):
        assert type(label) == str
        parser.output.write(ind + "if-goto " + label + '\n')

    def writeCall(name, argnumbers):
        assert type(name) == str
        assert type(argnumbers) == int
        parser.output.write(ind + "call " + name + " " + 
                                     str(argnumbers) + '\n')

    def writeFunction(name, varnumbers):
        assert type(name) == str
        assert type(varnumbers) == int
        parser.output.write("function " + filename + 
                                     '.' + name + " " + str(varnumbers) + '\n')

    def writeReturn():
        parser.output.write(ind + 'return\n')

    def closeVM():
        parser.output.close()
        

    def compileClass(token, index):
        token = token.next()
        assert token.kind in ['identifier', 'keyword']
        #print directory
        VMwriter(token.name, directory)
        token = token.next()
        assert token.kind == 'symbol' and token.name == '{'
        token = token.next()
        if token.kind == 'keyword' and token.name in ['static', 'field']:
            compileClassVarDec(token, index+1)
            token = token.reset()
        while token.kind == 'keyword' and token.name in ['constructor', 
                            'function', 'method']:
            compileSubroutine(token, index+1)
            token = token.next()
        assert token.kind == 'symbol' and token.name == '}'
        closeVM()
        return  

    def compileClassVarDec(token, index):
        while token.kind == 'keyword' and token.name in ['static', 'field']:
            kind = token.name
            token = token.next()
            assert token.kind in ['keyword', 'identifier']
            types = token.name
            token = token.next()
            if token.kind != 'symbol' or token.name != ';':
                assert token.kind in ['keyword', 'identifier']
                name = token.name
                token.text.tokenbegin(name, types, kind)
                token = token.next()
                while token.kind == 'symbol' and token.name == ',':
                    token = token.next()
                    assert token.kind in ['keyword', 'identifier']
                    name = token.name
                    token.text.tokenbegin(name, types, kind)
                    token = token.next()
            assert token.kind == 'symbol' and token.name == ';'
            token = token.next()
        return 
        
    def compileSubroutine(token, index):
        token.text.init()
        routine_type = token.name
        token = token.next()
        assert token.kind in ['keyword', 'identifier']
        return_type = token.name
        token = token.next()
        assert token.kind == 'identifier'
        name = token.name
        token = token.next()
        assert token.kind == 'symbol' and token.name == '('
        token = token.next()
        if routine_type == 'method':
            token.text.tokenbegin('this', filename, 'arg')
        compileParameterList(token, index+1)
        token = token.reset()
        assert token.kind == 'symbol' and token.name == ')'
        token = token.next()
        assert token.kind == 'symbol' and token.name == '{'
        token = token.next()
        while token.kind == 'keyword' and token.name == 'var':
            compileVarDec(token, index+2)
            token = token.reset()
        varnumbers = token.text.variables('var')
        writeFunction(name, varnumbers)
        if routine_type == 'method':
            writePush('argument', 0)
            writePop('pointer', 0)
        if routine_type == 'constructor':
            writePush('constant', 
                token.text.variables('var') + token.text.variables('field'))
            writeCall('Memory.alloc', 1)
            writePop('pointer', 0)
        compileStatements(token, index + 2)
        token = token.reset()
        assert token.kind == 'symbol' and token.name == '}'
        return 
        
    def compileParameterList(token, index):
        kind = 'arg'
        run_through_once = False
        while run_through_once == False or (
                token.kind == 'symbol' and token.name == ','):
            if token.kind == 'symbol' and token.name == ')':
                break
            if token.kind == 'symbol' and token.name == ',':
                token = token.next()
            assert token.kind in ['keyword', 'identifier']
            types = token.name
            token = token.next()
            assert token.kind == 'identifier'
            name = token.name
            token.text.tokenbegin(name, types, kind)
            token = token.next()
            run_through_once = True
        return 

    def compileVarDec(token, index):
        run_through_once = False
        kind = token.name
        token = token.next()
        assert token.kind in ['keyword', 'identifier']
        types = token.name
        token = token.next()
        if token.kind != 'symbol' or token.name != ';':
            assert token.kind in ['keyword', 'identifier']
            name = token.name
            token.text.tokenbegin(name, types, kind)
            token = token.next()
            while token.kind == 'symbol' and token.name == ',':
                token = token.next()
                assert token.kind == 'identifier'
                name = token.name
                token.text.tokenbegin(name, types, kind)
                token = token.next()
        assert token.kind == 'symbol' and token.name == ';'
        token = token.next()
        return 

    def compileStatements(token, index):
        while token.kind != 'symbol' or token.name != '}':
            assert token.kind == 'keyword' and token.name in [
                                    'let', 'if', 'while', 'do', 'return']
            if token.name == 'let':
                compileLet(token, index+1) 
            elif token.name == 'if':
                compileIf(token, index+1) 
            elif token.name == 'while':
                compileWhile(token, index+1) 
            elif token.name == 'do':
                compileDo(token, index+1) 
            elif token.name == 'return':
                compileReturn(token, index+1) 
            token = token.reset()
        return 

    def compileDo(token, index):
        argnumbers = 0
        token = token.next()
        assert token.kind == 'identifier'
        name = token.name
        token = token.next()
        assert token.kind == 'symbol' and token.name in ['(', '.']
        if token.name == '.':
            token = token.next()
            assert token.kind == 'identifier'
            if (name in token.text.Subroutine or
                    name in token.text.Class):
                segment = segmentdict[token.text.tokenK(name)]
                index = token.text.TokenI(name)
                writePush(segment, index)
                argnumbers += 1
                name = token.text.tokenT(name) + '.' + token.name
            else:
                name = name + '.' + token.name
            token = token.next()
        else:
            writePush('pointer', 0)
            argnumbers += 1
            name = filename + '.' + name
        assert token.kind == 'symbol' and token.name == '('
        token = token.next()
        argnumbers = argnumbers + compileExpressionList(token, index + 1)
        writeCall(name, argnumbers)
        writePop('temp', 0)
        token = token.reset()
        assert token.kind == 'symbol' and token.name == ')'
        token = token.next()
        assert token.kind == 'symbol' and token.name == ';'
        token = token.next()
        return 

    def compileLet(token, index):
        is_array = False
        token = token.next()
        assert token.kind == 'identifier'
        var_to_tokenbegin = token.name
        if token.run().kind == 'symbol' and token.run().name == '[':
            is_array = True
            token = token.next()
            token = token.next()
            if token.kind != 'symbol' or token.name != ']':
                compileExpression(token, index + 1)
                token = token.reset()
            else:
                writePush('constant', 0)
            writePop('temp', 1)
            assert token.kind == 'symbol' and token.name == ']'
            token = token.next()
        else:
            token = token.next()
        assert token.kind == 'symbol' and token.name == '='
        token = token.next()
        compileExpression(token, index + 1)
        token = token.reset()
        assert token.kind == 'symbol' and token.name == ';'
        if is_array:
            writePush(segmentdict[token.text.tokenK(var_to_tokenbegin)],
                      token.text.TokenI(var_to_tokenbegin))
            writePush('temp', 1)
            writeArithmetic('add')
            writePop('pointer', 1)
            writePop('that', 0)
        else:
            writePop(segmentdict[token.text.tokenK(var_to_tokenbegin)], 
                     token.text.TokenI(var_to_tokenbegin))
        token = token.next()
        return         

    def compileWhile(token, index):
        label_name = filename + str(token.index)
        token = token.next()
        assert token.kind == 'symbol' and token.name == '('
        token = token.next()
        writeLabel("WHILE_EXP"+label_name)
        compileExpression(token, index + 1)
        writeArithmetic('not')
        writeIf("WHILE_END"+label_name)
        token = token.reset()
        assert token.kind == 'symbol' and token.name == ')'
        token = token.next()
        assert token.kind == 'symbol' and token.name == '{'
        token = token.next()
        #writeLabel("START"+label_name)
        compileStatements(token, index + 1)
        token = token.reset()
        assert token.kind == 'symbol' and token.name == '}'
        writeGoto("WHILE_EXP"+label_name)
        writeLabel("WHILE_END"+label_name)
        token = token.next()
        return 

    def compileReturn(token, index):
        token = token.next()
        if token.kind != 'symbol' or token.name != ';':
            compileExpression(token, index)
        else:
            writePush('constant', 0)
        token = token.reset()
        assert token.kind == 'symbol' and token.name == ';'
        writeReturn()
        token = token.next()
        return 
        
    def compileIf(token, index):
        label_name = filename + str(token.index)
        token = token.next()
        assert token.kind == 'symbol' and token.name == '('
        token = token.next()
        compileExpression(token, index + 1)
        writeIf("IF_TRUE"+label_name)
        writeGoto("IF_FALSE"+label_name)
        token = token.reset()
        assert token.kind == 'symbol' and token.name == ')'
        token = token.next()
        assert token.kind == 'symbol' and token.name == '{'
        token = token.next()
        writeLabel("IF_TRUE"+label_name)
        compileStatements(token, index + 1)
        token = token.reset()
        assert token.kind == 'symbol' and token.name == '}'
        if (token.run()).kind == 'keyword' and (
                token.run()).name == 'else':
            writeGoto("IF_END"+label_name)
            writeLabel("IF_FALSE"+label_name)
            token = token.next()
            token = token.next()
            assert token.kind == 'symbol' and token.name == '{'
            token = token.next()
            compileStatements(token, index + 1)
            token = token.reset()
            assert token.kind == 'symbol' and token.name == '}'
            writeLabel("IF_END"+label_name)
        else:
            writeLabel("IF_FALSE"+label_name)
        token = token.next()
        return 
        
    def compileExpressionList(token, index):
        params = 0
        run_through_once = False
        while run_through_once == False or (token.kind == 'symbol' and 
                                            token.name == ','): 
            if token.kind == 'symbol' and token.name == ')':
                break
            if token.kind == 'symbol' and token.name == ',':
                token = token.next()
            compileExpression(token, index + 1)
            params += 1
            token = token.reset()
            run_through_once = True
        return params

    def compileExpression(token, index):
        compileTerm(token, index + 1)
        token = token.reset()
        while token.kind == 'symbol' and token.name in ops:
            operation = token.name
            token = token.next()
            compileTerm(token, index + 1)
            if operation in ['+', '-', '=', '&gt;', '&lt;', '&amp;', '|']:
                writeArithmetic(ops[operation])
            elif operation == '*':
                writeCall('Math.multiply', 2)
            elif operation == '/':
                writeCall('Math.divide', 2)
            token = token.reset()
        return 
            
    def compileTerm(token, index):
        is_array = False
        if token.kind == 'symbol':
            if token.name in unaryops:
                operation = token.name
                token = token.next()
                compileTerm(token, index + 1)
                writeArithmetic(unaryops[operation])
            elif token.name == '(':
                token = token.next()
                compileExpression(token, index + 1)
                token = token.reset()
                assert token.kind == 'symbol' and token.name == ")"
                token = token.next()
        elif token.kind == 'keyword':
            assert token.name in keywords
            if token.name == 'true':
                writePush('constant', 1)
                writeArithmetic('neg')
            else:
                if token.name in ['false', 'null']:
                    segment = 'constant'
                else:
                    segment = 'pointer'
                writePush(segment, 0)
            token = token.next()
        elif token.kind in ['identifier', 'integerConstant', 'stringConstant']:
            name = token.name
            if token.kind == 'integerConstant':
                writePush('constant', int(name))
            elif token.kind == 'stringConstant':
                string_len = len(token.name)
                writePush('constant', string_len)
                writeCall('String.new', 1)
                for i in range(0, string_len):
                    writePush('constant', ord(token.name[i]))
                    writeCall('String.appendChar', 2) 
            else:
                if token.text.tokenK(token.name) == None:
                    segment = 0 # Results in error - should ever get there?
                else:
                    segment = segmentdict[token.text.tokenK(token.name)]
                    index = token.text.TokenI(token.name)
                    writePush(segment, index)
            nexttoken = token.run()
            if nexttoken.kind == 'symbol':
                if nexttoken.name == '[':
                    is_array = True
                    token = token.next()
                    token = token.next()
                    if token.kind != 'symbol' or token.name == ']':
                        compileExpression(token, index + 1)
                        token = token.reset()
                    else:
                        writePush('constant', 0)
                    writeArithmetic('add')
                    writePop('pointer', 1)
                    writePush('that', 0)
                    assert token.kind == 'symbol' and token.name == ']'
                elif nexttoken.name in ['(', '.']:
                    argnumbers = 0
                    if nexttoken.name == '.':
                        if (token.name in token.text.Subroutine or
                                token.name in token.text.Class):
                            segment = segmentdict[token.text.tokenK(token.name)]
                            argnumbers += 1
                            index = token.text.TokenI(token.name) 
                            #writePush(segment, index)
                            token = token.next()
                            token = token.next()
                            name = token.text.tokenT(
                                    name) + '.' + token.name
                        else:
                            token = token.next()
                            token = token.next()
                            name = name + '.' + token.name
                        assert token.kind == 'identifier'
                        token = token.next()
                        assert token.kind=='symbol' and token.name=='('
                    else:
                        writePush('pointer', 0)
                        token = token.next()
                        name = filename + '.' + token.name
                    token = token.next()
                    argnumbers += compileExpressionList(token, index + 1)
                    writeCall(name, argnumbers)
                    token = token.reset()
                    assert token.kind == 'symbol' and token.name == ')'
            token = token.next()
        return 
                                
    

    index = 0
    for token in text.tokens:
        if token.name == 'class' and token.kind == 'keyword':
            compileClass(token, index)
        text.cursor += 1
    return ''.join(newtext)


if __name__ == '__main__':
    if len(sys.argv)==2:
        pathname = sys.argv[1]
        if pathname[-1] == '/':
            pathname = pathname[:-1]

        if '/' not in pathname:
            tag = 0
        else:
            tag = pathname.rfind('/')
        #input single file
        if '.' in pathname[tag:]:
            if pathname[-5:] == '.jack':
                process_file(pathname)
            else:
                print "You have to input a .jack filename or directory!"
                raise NameError
        #input a directory
        else:
            process_directory(pathname)
    else:
        print "You have to input a .jack filename or directory!"
        raise NameError
