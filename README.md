# Watch Diary v1.0

## Overview

   Watch Diary is a personalized media-tracking application that allows users to catalog their consumption of movies and TV episodes. Inspired by the need to keep track of media I've watched or played, this application provides a centralized way to log, view, and manage details about films and TV shows.
   The program features intuitive UI windows with dedicated menus for adding, updating, and searching entries. Users can organize and input metadata such as titles, genres, release dates, watch/play dates, and personal ratings. This makes it easy to reflect on past media engagement and manage future entries.

## Key Features

   - Database Integration: Uses SQLite to create and maintain a custom database file that stores all entries.
   - API Integration: Retrieves media details automatically via the TMDb API using IMDb IDs, reducing the need for manual input.
   - User Interface: Built with Qt, the UI provides an accessible and organized experience:
   - Add films and TV episodes using forms with integrated search functionality.
   - View and edit entries using an editable table, with filters and search capabilities.
   - Reset the database with a confirmation prompt to start fresh if needed.
   - About Page: Includes program details and version information.

## Development Process

   ### 1. Database Design
   - The application creates and connects to an SQLite database every time it is run. Tables are structured to store all necessary media information, ensuring data consistency and ease of access.
   ### 2. Adding Films and TV Episodes
   To streamline the data entry process:
   - Films: Users can enter an IMDb ID to retrieve basic details. After entering additional watch session information, the data is saved to the database.
   - TV Shows: Fetching TV show and episode details required dual API calls to retrieve comprehensive information. While the automated insertion process is incomplete, users can view the fetched details and manually input them into the database.
   ### 3. Viewing and Editing Entries
   The program includes a table view with the following features:
   - Filters: Switch between viewing films and TV episodes.
   - Search: Find specific entries by title or metadata.
   - Editable Fields: Update details directly within the table.
   - Delete Functionality: Remove unwanted entries as needed.
   ### 4. Future Enhancements
   Potential improvements include:
   - Displaying detailed media information in a dedicated window with poster images and metadata.
   - Integrating trailers and visual media, such as director and actor photos, studio logos, etc., for an enriched user experience.
   ### 5. Challenges
   - API Integration: Hardcoding the API key and URL is a limitation of the current implementation, but it was necessary for demonstration purposes.
   - Video Game Tracking: While initially planned, this feature was excluded due to complexities in API handling and the availability of tracking on modern consoles.
   - Cross-Platform Application Icon: Due to differences in icon assignment between Windows and macOS, the project currently uses a blank default icon.

## Demo

### 🎥 [Video Demonstration](https://youtu.be/bgpUe4TAv6k)
This demo showcases the application’s key functionalities. (No audio track is included.)
