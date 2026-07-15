*This project has been created as part of the 42 curriculum by emiconte.*

# NETPRACTICE

## Description

NetPractice is a project to learn the basics of networking: IP addressing, subnet masks, gateways, and how to make machines communicate with each other through routers and switches.
The goal is to fix, level by level, broken network configurations until all hosts can properly communicate with each other.

## Instructions

- Download the file provided on the project page, extract it, then run `run.sh`: it opens the interface in your browser
(if it doesn't work, start a local server with `python3 -m http.server` and open the corresponding URL).
- Enter your login to use your personal configuration, or use the "evaluation" tab to generate a random configuration.
- Fix the configuration for each level until the objectives show OK, then export the file using **Get my config**.

## Resources

### Technical References

- Networking concepts studied: TCP/IP addressing, subnet masks, default gateways, routers and switches, OSI layers.
- PDF = Apprenez le fonctionnement des réseaux TCP/IP by Romain Guichard (Caelifer), elalitte
- YOUTUBE = NetPractice: An Intro to IP Addresses and Subnets [https://www.youtube.com/watch?v=HQUw0CfQWAM]

### AI Usage

AI (Claude, Anthropic) was used to help us understand certain technical points (the role of the router, the switch, subnet masks, routes, and why some routes caused loops).

## Submission

10 configuration files must be exported (one per level, using the **Get my config** button) and placed at the root of the Git repository.