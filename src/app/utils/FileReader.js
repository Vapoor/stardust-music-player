'use client'
import React, { useState } from "react";

export function SetInputReader() {
  const [folderPath, setFolderPath] = useState("");

  // Function to handle folder selection
  const handleFolderSelection = (event) => {
    const files = event.target.files;

    if (files.length > 0) {
      // Extract the full folder path from the first file
      const firstFile = files[0];
      const fullPath = firstFile.webkitRelativePath;
      const folder = fullPath.substring(0, fullPath.lastIndexOf("/")); // Get the folder part
      setFolderPath(folder);
      console.log("Selected folder path:", fullPath);
    }
  };

  return (
    <div>
      {/* File input with directory selection */}
      <input
        type="file"
        webkitdirectory="true"
        directory="true"
        onChange={handleFolderSelection}
      />
      {folderPath && <p>Selected Folder: {folderPath}</p>}
    </div>
  );
}


