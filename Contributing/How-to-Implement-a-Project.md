## Project Implementation  

This process assumes that you already have [started a New Project](https://github.com/moja-global/.github/blob/master/Contributing/How-to-Start-a-New-Project.md) and have an approved [Science Design](https://github.com/moja-global/.github/blob/master/Contributing/How-to-Agree-on-a-Science-Design.md) 

1. Confirm Your Team  
    * Contact the maintainers and documenters of the repository you want to code into and discuss your project. Agree on write and maybe admin permissions. Assign additional maintainers and documentation writers as needed. 
    * Assign a Code Coach who is willing to introduce new coders to the project. 
    * Publicise widely that the project is ready to code and is looking for coders, documentation writers, testers, etc. When moja global users collaborate everybody wins. Contact info@moja.global to get support with reaching out to the community of users
    * Emphasize to all new contributors to review the [Coding Guidelines](https://github.com/moja-global/.github/blob/master/Governance/Coding-Guidelines.md) and the [Contribution Criteria](https://github.com/moja-global/.github/blob/master/Governance/Contribution-Criteria.md).   

1. User Stories and Minimum Viable Product
    * Use User stories to translate a scientific design into a codable task list. 
    * When the scientific design is approved, organise a conference call with all the science contributors (at a minimum the maintainers of the Science Design have to be present) and the interested coders (at a minimum the maintainers of the future code should be present) to contribute to the project. Real time interaction is important so better fewer participants on a call than more participants through a written document.
    * The coders can ask clarifications from the scientists to ensure that every aspect of the Science Design is clear. 
    * Next coders and scientists individually draft all the user stories they can think of into the GitHub Project Board. All user stories will have the following syntax: As a [User, Admin, …], I can [action: click, scroll, …] to [result: calculate, open, load, erase, …]
    * After a first drafting session, the User Stories are classified as ‘essential’ or ‘later’. All the User Stories classified as essential must be part of the Minimum Viable Product (MVP), i.e. none of them can be dropped without breaking the ‘why’ of the project. Check every user story several times to check whether it can be dropped from the MVP.
    * All the User Stories are moved to the “To do” column of the GitHub Project Board. The MVP stories are moved to “In progress”. 

1. Agile Architecture  
    * Agree on an initial project architecture with the software team (this improves communication, increases speed and quality of the coding and reduces risks)
    * There are no fixed rules about what you should define up front. Your minimum outcome is that everybody agrees how the project should be built and how it should fit into the wider software components. 
    * It is logical to define the interactions with the existing system and sketch a technology diagram linking user interface with the business process and with the data. 
    * Use [branding](https://github.com/moja-global/.github/blob/master/Governance/Branding.md) and [User Interface Style](https://github.com/moja-global/.github/blob/master/Governance/User-Interface-Style.md) of the wider software components. 

1. Iterative Build
    * Built the MVP in the initial sprint. 
    * Document the code in the code base and in the wiki. (Everybody can edit the wiki. So if you want to create documentation that need maintainer approval for changes, use regular .md files.)
    * Develop tests (in collaboration with Scientists) to check whether the software is working accurately.  The code is tested continuously based on these tests. The tests are upgraded continuously with the code. 
    * Invite feedback from users after every sprint. The feedback is used to improve documentation in the wiki and to update the backlog. 
    * Take the feedback into account when you prioritize the backlog and start a new sprint. 
    * Continue this cycle until the project is ready for a first release. 

1. Approve Release
    * Once the code is ready for release, document the tests that have been performed, copy the wiki to the repository and create a [release-candidate version](https://github.com/moja-global/.github/blob/master/Contributing/How-to-Assign-a-Version.md) which includes the wiki and all other documentation. 
    * Submit the pre-release to the scientists and code maintainers for final review. Invite the independent scientists to participate in the review if a science review panel was used. Document test results and feedback from the reviewers and maintainers.
    * Complete all tests and bug-fixes 
    * Submit the pre-release with a pull-request, indicate that you want [@TSC to review your pull-request](https://help.github.com/en/articles/requesting-a-pull-request-review).
    * The project will be released with agreed version number
    
    
