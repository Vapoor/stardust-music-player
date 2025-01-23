import React from 'react';
import {SetInputReader} from './utils/FileReader';
import {DarkMode} from './utils/Darkmode';

export default function Home(){
  return (
    <React.StrictMode>
    <html>
      <body>
      
    <section id="welcome">
      <h1>Welcome :)</h1>
    </section>
    <SetInputReader/>
      </body>
    </html>
    </React.StrictMode>
  )
}