/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce::build_tools
{

    //==============================================================================
    /** Manipulates a cross-platform partial file path. (Needed because File is designed
        for absolute paths on the active OS)
    */
    class RelativePath
    {
    public:
        //==============================================================================
        enum RootFolder
        {
            unknown,
            projectFolder,
            buildTargetFolder
        };

        //==============================================================================
        RelativePath()
            : root (unknown)
        {}

        RelativePath (const String& relPath, const RootFolder rootType)
            : path (unixStylePath (relPath)), root (rootType)
        {}

        RelativePath (const File& file, const File& rootFolder, const RootFolder rootType)
            : path (unixStylePath (getRelativePathFrom (file, rootFolder))), root (rootType)
        {}

        RootFolder getRoot() const                              { return root; }

        String toUnixStyle() const                              { return unixStylePath (path); }
        String toWindowsStyle() const                           { return windowsStylePath (path); }

        String getFileName() const                              { return getFakeFile().getFileName(); }
        String getFileNameWithoutExtension() const              { return getFakeFile().getFileNameWithoutExtension(); }

        String getFileExtension() const                         { return getFakeFile().getFileExtension(); }
        bool hasFileExtension (StringRef extension) const       { return getFakeFile().hasFileExtension (extension); }
        bool isAbsolute() const                                 { return isAbsolutePath (path); }

        RelativePath withFileExtension (const String& extension) const
        {
            return RelativePath (path.upToLastOccurrenceOf (".", ! extension.startsWithChar ('.'), false) + extension, root);
        }

        RelativePath getParentDirectory() const
        {
            String p (path);
            if (path.endsWithChar ('/'))
                p = p.dropLastCharacters (1);

            return RelativePath (p.upToLastOccurrenceOf ("/", false, false), root);
        }

        RelativePath getChildFile (const String& subpath) const
        {
            if (isAbsolutePath (subpath))
                return RelativePath (subpath, root);

            String p (toUnixStyle());
            if (! p.endsWithChar ('/'))
                p << '/';

            return RelativePath (p + subpath, root);
        }

        RelativePath rebased (const File& originalRoot, const File& newRoot, const RootFolder newRootType) const
        {
            if (isAbsolute())
                return RelativePath (path, newRootType);

            return RelativePath (getRelativePathFrom (originalRoot.getChildFile (toUnixStyle()), newRoot), newRootType);
        }

    private:
        //==============================================================================
        String path;
        RootFolder root;

        File getFakeFile() const
        {
            const auto unixStylePath = toUnixStyle();
            const auto name = unixStylePath.substring (unixStylePath.lastIndexOfChar ('/') + 1);

            // This method gets called very often, so we'll cache this directory.
            static const File currentWorkingDirectory (File::getCurrentWorkingDirectory());
            return currentWorkingDirectory.getChildFile (name);
        }
    };

} // namespace juce::build_tools
