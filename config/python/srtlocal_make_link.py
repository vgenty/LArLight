import os,sys,commands

# check if SRT_LOCAL & LAR_MODULE are defined. If not, nothing to do.
if not 'SRT_LOCAL' in os.environ or not 'LAR_MODULE' in os.environ:

    sys.exit(1)

# Get a list of LArSoft packages to be included under $SRT_LOCAL
if "LAR_MODULE" in os.environ:

    packages = ['%s/%s' % (os.environ['LAR_MODULE_DIR'],x) for x in os.environ['LAR_MODULE'].split(None)]

    valid_packages=[]
    for package in packages:
        if os.path.isdir(package):
            valid_packages.append(package)

    # create a softlink
    for package in valid_packages:
        os.system('ln -sf %s %s/' % (package,os.environ['SRT_LOCAL']))
    
