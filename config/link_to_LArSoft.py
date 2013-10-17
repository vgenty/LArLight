import os,sys,commands

make_link   = False;
target_link = "%s/DataScanner" % sys.argv[1]
source_link = "%s/DataScanner" % os.environ['MAKE_TOP_DIR']
if os.path.islink(target_link):
    current_link=commands.getoutput("ls -l %s" % target_link)
    current_link=current_link.split(None)[len(current_link.split(None))-1]
    if not current_link == source_link:
        print "Found already existing link %s\n    => %s" % (target_link,current_link)
        print
        print "Can we replace it to: %s\n    => %s" % (target_link,current_link)
        user_input=''
        while not user_input in ['y','n']:
            if user_input:
                print 'Invalid input: %s' % user_input
            sys.stdout.write("[y/n]:")
            sys.stdout.flush()
            user_input = sys.stdin.read().rstrip('\n')
        if user_input == 'y':
            os.system('rm %s' % target_link)
            if os.path.islink(target_link):
                print 'ERROR:'
                print 'Failed to replace a link %s' % target_link
                print 'BE AWARE!'
            else:
                make_link=True
        else:
            print 'INFO:'
            print 'Intentionally keeping a softlink NOT POINTING TO THIS INSTALLATION!'
            print 'BE AWARE!'
elif os.path.isdir(target_link):
    print "ERROR:"
    print "Physica directory exists: %s" % target_link
    print "in place where we need to make a softlink!"
    print "BE AWARE!"
else:
    make_link=True
#    print "WARNING:"
#    print "Attempting to make a soft-link: %s" % target_link
#    print "But a physical directory already exists. Aborting this step."
#    print

if make_link:
    os.system("ln -s %s %s" % (source_link,target_link))
    if not os.path.islink(target_link):
        print "ERROR:"
        print "Failed to create a softlink: %s" % target_link
        print "BE AWARE!"
        print
        sys.exit(1)

    # Make a soft-link to compile LArLight
    source_link = os.environ['ANA_PROC_DIR']
    target_link = "%s/AnaProcess" % target_link
    if not os.path.islink(target_link):
        os.system("ln -s %s %s" % (source_link, target_link))
    if not os.path.islink(target_link):
        print "ERROR:"
        print "Failed to create a softlink: %s" % target_link
        print "BE AWARE!"
        print
        sys.exit(1)

        


    
