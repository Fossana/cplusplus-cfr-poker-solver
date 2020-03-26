import sys

id = 0

def fileToLineInfoArray( infile ):
        with open( infile, 'r' ) as data:
                lines = data.readlines()

        return [ ( len( line.split('\t') ) - 1, line.strip() ) for line in lines ]

def makeId():
        global id

        id += 1
        return 'n{}'.format( id )

def buildTrees( lines, lastIndent = 0, lastParentId = None ):
        i = 0

        trees = []

        if 0 == lastIndent:
                lines = filter( lambda l: l[0] > 0 or l[1] == 'tree', lines )

        while i < len( lines ):
                if lines[i][0] == lastIndent:
                        if 0 == lastIndent:
                                parentId = None
                        else:
                                parentId = makeId()
                                trees.extend( [ [ parentId, lines[i][1], lastParentId, lastIndent ] ] )
                        i += 1
                        continue

                nextLines = [ lines[i] ]
                i += 1
                while i < len( lines ) and not lines[i][0] == lastIndent:
                        nextLines.append( lines[i] )
                        i += 1

                if len( nextLines ) > 1:
                        trees.extend( buildTrees( nextLines, lastIndent + 1, parentId ) )
                else:
                        trees.extend( [ [ makeId(), nextLines[0][1], parentId, lastIndent + 1 ] ] )

        return trees

def generateGraph( trees ):
        print 'digraph tree {'
        print '\tratio=0.35;'
        print '\tnode [color=black];'
        print '\tnode [shape=none];'
        #print '\tedge [arrowhead=none];'

        # Go one rank at a time
        rank = 1
        while True:
                rankNodes = filter( lambda x: x[3] == rank, trees )
                if len( rankNodes ) == 0:
                        break

                nodeList = []
                edgeList = []

                for node in rankNodes:
                        nodeLabel = node[1].split(' - ')
                        weight=1;
                        if(len(nodeLabel)>1):
                            weight=float(nodeLabel[1])*5;
                        nodeList.append( '{}[label="{}",penwidth={}];'.format( node[0], nodeLabel[0],weight ) )
                        #nodeLabel = "\n".join( node[1].split(' - ') )
                        #nodeList.append( '{}[label="{}"];'.format( node[0], nodeLabel ) )
                        if not node[2] is None:
                                edgeList.append( '{} -> {} [label={},penwidth={}];'.format( node[2], node[0],weight/5,weight) )

                print '{{rank=same; {}}};'.format( ' '.join( nodeList ) )
                print '\n'.join( edgeList )
                
                rank += 1
        print '}'

def main():
        infile = sys.argv[1]

        lines = fileToLineInfoArray( infile )

        trees = buildTrees( lines )

        generateGraph( trees )

if "__main__" == __name__:
        main()
