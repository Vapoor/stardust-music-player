import React from 'react';
import {SetInputReader} from './utils/FileReader';
import {DarkMode} from './utils/Darkmode';

export default function Home(){
  return (
    <React.StrictMode>
    <section id="welcome">
      <h1>Welcome :)</h1>
    </section>
    <SetInputReader/>
    </React.StrictMode>
  )
}