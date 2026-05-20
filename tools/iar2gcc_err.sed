#Jump target
:a
#Detect 1st line of an error
/^"\(.*\(\\\|\/\).*\..*\)"\,\([[:digit:]]\{1,\}\) *\(Error\|Warning\|Fatal error\)\[[[:alnum:]]\{1,\}\]: \(.*\)/ {
    #Jump target
    :b
    #If not end of file
    $! {
    	#Read in next line
    	N
        #If the line we just read in is not blank, then we need to get more lines of this error
        /\n$/! bb
    }
    #Combine all error lines into one
    s/\n[ ]*/ /g
    #Reformat the error into gcc syntax
    s/^"\(.*\(\\\|\/\).*\..*\)"\,\([[:digit:]]\{1,\}\) *\(Error\|Warning\)\[[[:alnum:]]\{1,\}\]: \(.*\)/\1:\3: \4: \5/
    #If not end of file, append back the empty line we consumed
    $! a
    #Print out what we've done (the pattern space we've been working in)
    p
    #Delete what we've done, so we don't find this same error again
    d
}
#If there are more errors, process them
/^"\(.*\(\\\|\/\).*\..*\)"\,\([[:digit:]]\{1,\}\) *\(Error\|Warning\)\[[[:alnum:]]\{1,\}\]: \(.*\)/ ba