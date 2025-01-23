'use client'
import { useState, useEffect } from "react";

export function DarkMode({Component,pageProps}){
    const [isDarkMode, setIsDarkMode] = useState(false);

    // Load saved preference from localStorage
    useEffect(() => {
      const storedTheme = localStorage.getItem("theme");
      if (storedTheme === "dark") {
        document.documentElement.classList.add("dark");
        setIsDarkMode(true);
      }
    }, []);
  
    // Toggle dark mode
    const toggleDarkMode = () => {
      const newMode = !isDarkMode;
      setIsDarkMode(newMode);
      if (newMode) {
        document.documentElement.classList.add("dark");
        localStorage.setItem("theme", "dark");
      } else {
        document.documentElement.classList.remove("dark");
        localStorage.setItem("theme", "light");
      }
    };
  
    return (
      <div>
        <button
          onClick={toggleDarkMode}
          className="p-2 rounded bg-gray-200 dark:bg-gray-700 dark:text-white"
        >
          {isDarkMode ? "Light Mode" : "Dark Mode"}
        </button>
        <Component {...pageProps} />
      </div>
    );
}