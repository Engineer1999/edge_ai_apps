# Build include file for Documentation
#
# Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/
# ALL RIGHTS RESERVED
#

# Sphinx Config
DOC_VERSION   = $(shell printf "%02d.%02d.%02d" $(VERSION) $(SUBVERSION) $(PATCHVERSION))
VEROPTS       = -D version=$(DOC_VERSION) -D release=$(DOC_VERSION)
SPHINXOPTS    = $(VEROPTS) -n -W
SPHINXBUILD   = sphinx-build
PAPER         =
BUILDDIR      = build
# User-friendly check for sphinx-build
ifeq ($(shell which $(SPHINXBUILD) >/dev/null 2>&1; echo $$?), 1)
$(warning The '$(SPHINXBUILD)' command was not found. Make sure you have Sphinx installed, then set the SPHINXBUILD environment variable to point to the full path of the '$(SPHINXBUILD)' executable. Alternatively you can add the directory with the executable to your PATH. If you don't have Sphinx installed, grab it from http://sphinx-doc.org/)
endif

# Sphinx Internal variables.
PAPEROPT_a4     = -D latex_paper_size=a4
PAPEROPT_letter = -D latex_paper_size=letter
ALLSPHINXOPTS   = $(PAPEROPT_$(PAPER)) $(SPHINXOPTS)
# the i18n builder cannot share the environment and doctrees with the others
I18NSPHINXOPTS  = $(PAPEROPT_$(PAPER)) $(SPHINXOPTS)

docs-help:
	@echo "Please use \`make <target>' where <target> is one of"
	@echo "  docs          to make standalone HTML files for full documentation set"

docs:
	$(SPHINXBUILD) -b html -d $(BUILDDIR)/doctrees $(ALLSPHINXOPTS) . $(BUILDDIR)/html
	@echo
	@echo "Build finished. The HTML pages are in $(BUILDDIR)/html."

check-words:
	@echo "Checking reserved words"
	ti_resw -db ep_proc_sw.json -D $(srctree) -V2 -R report -c

clean_docs:
	rm -rf $(BUILDDIR)/*

ALL_DOCRULES := docs

alldocs: $(ALL_DOCRULES)
	@echo "All project documents have been generated."

PHONY += $(ALL_DOCRULES) clean_docs
