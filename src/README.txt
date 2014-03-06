This is the README file for Perforce's p4win open source project.

To build you will need:
    - p4-win source and build files from the Perforce Workshop
    - latest 13.3 (change 784164 or later) p4api.


The source is shipped with Visual Studio 2010 solution and project files.

1. p4 sync p4win source from the Perforce Workshop
    * set read/write perms on the following files in gui:
       P4Win.sln
       P4Win.vcxproj
       P4EnvVariables.props
       P4Win409/P4Win409.vcxproj
       P4Win411/P4Win411.vcxproj

2  download the p4api from the Perforce ftp/website.
    http://ftp.perforce.com/perforce/r13.3/
    p4api-2013.3.784164 or higher

3. load the p4-win/gui/P4Win.sln file into Visual Studio 2010.

    * if Visual Studio complains that the project file has been renamed 
      or is no longer in the solution you might have to manually correct the
      dependency GUID's in the P4win.vcxproj file.

      Remove and Re-add the project files
      ----------------------------------------
         - Open the Solution Explorer and expand P4Win solution
         - Select P4Win project 
         - Bring up right context menu and select Remove 
         - Repeat to Remove the P4Win409 and P4Win411 projects 
         - Add the projects back into the solution
            * Select P4Win Solution 
            * right context menu Add->Existing Project...
            * load P4Win.vcxproj
            * load P4Win409/P4Win409.vcxproj
            * load P4Win411/P4Win411.vcxproj

      If that doesn't work, close the solution and manually edit the P4win.vcxproj file.

      Manually correct GUIDs
      ----------------------
         - open the p4-win/gui/P4win.sln file
           Copy the dependency GUID's in the sln file into the P4win.vcxproj file

           example:

                Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "P4Win411", 
                          "P4Win411\P4Win411.vcxproj", 
                          "{A5411300-4E8E-F0A2-FC25-8A1EE61C86E2}" 
                EndProject

            The GUID for P4Win411 is A5411300-4E8E-F0A2-FC25-8A1EE61C86E2

            Paste this GUID into the P4win.vcxproj file's Project Reference 
            for P4Win409:

            example:

                  <ItemGroup>
                    <ProjectReference Include="P4Win409\P4Win409.vcxproj">
                      <Project>{CF4BDD1A-C381-5F38-E7CE-7AD0B5322BE0}</Project>
                      <ReferenceOutputAssembly>false</ReferenceOutputAssembly>
                    </ProjectReference>
                    <ProjectReference Include="P4Win411\P4Win411.vcxproj">
                      <Project>{A5411300-4E8E-F0A2-FC25-8A1EE61C86E2}</Project>
                      <ReferenceOutputAssembly>false</ReferenceOutputAssembly>
                    </ProjectReference>
                  </ItemGroup>
           
4. set the P4APIDIR environment variable 

    * View->Other Windows->Property Manager
    * Expand P4win->Unicode Release|Win 32
      Open up P4EnvVariables property page
      Expand Common Properties->User Macros
      set P4APIDIR to the location of the downloaded p4api

      ** alternatively you can directly edit gui/P4EnvVariables.props 
         and set the path there.

5. Set the Solution Build Configuration Properties

    View->Solution Explorer
    Select Solution 'P4Win'
    Right context menu -> Properties -> Configuration Properties
    Select the correct Project Configuration to match the p4api library
    (debug, Unicode, Release)

6.  To Build the Solution

    Right click and select 'Build Solution'

