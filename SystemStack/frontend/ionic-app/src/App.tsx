import { Redirect, Route } from 'react-router-dom';
import { useContext, useEffect, useState } from 'react';
import { IonApp, IonContent, IonHeader, IonIcon, IonLabel, IonPage, IonRouterOutlet, IonTabBar, IonTabButton, IonTabs, setupIonicReact } from '@ionic/react';
import { IonReactRouter } from '@ionic/react-router';
import { help, home, lockClosed, mail, people } from 'ionicons/icons';

import Chats from './pages/Chats';

/* Core CSS required for Ionic components to work properly */
import '@ionic/react/css/core.css';

/* Basic CSS for apps built with Ionic */
import '@ionic/react/css/normalize.css';
import '@ionic/react/css/structure.css';
import '@ionic/react/css/typography.css';

/* Optional CSS utils that can be commented out */
import '@ionic/react/css/padding.css';
import '@ionic/react/css/float-elements.css';
import '@ionic/react/css/text-alignment.css';
import '@ionic/react/css/text-transformation.css';
import '@ionic/react/css/flex-utils.css';
import '@ionic/react/css/display.css';

/* Theme variables */
import './theme/variables.css';
import Home from './pages/Home';
import Organizations from './pages/Organizations';
import axios from 'axios';
import AboutUs from './pages/AboutUs';
import Vault from './pages/Vault';
import React from 'react';
import AppAppBar from './components/AppAppBar';

export interface User {
  username: string;
  first_name: string;
  last_name: string;
  email: string;
  phone: string;
}

export const AuthContext = React.createContext(false);
export const URIContext = React.createContext("http://localhost:8000/");
export const UserContext = React.createContext<User | null>(null);

setupIonicReact();

const App: React.FC = () => {
  const [loggedIn, setLoggedIn] = useState(false);
  const backendURI = useContext(URIContext)
  const [userDetails, setUserDetails] = useState<User | null>(null);

  const checkAuthentication = async () => {
    try {
      const response = await axios.get(backendURI + 'check-authentication', { withCredentials: true });
      setLoggedIn(response.data.is_authenticated);
      fetchUserDetails();
    } catch (error) {
      console.error('Error checking authentication:', error);
    }
  };

  const fetchUserDetails = async () => {
    try {
      const response = await axios.get(backendURI + 'user', { withCredentials: true });
      setUserDetails(response.data[0].fields);
    } catch (error) {
      console.error('Error fetching User details', error);
    }
  }

  useEffect(() => {
    checkAuthentication();
  }, []);

  return (
    <UserContext.Provider value={userDetails}>
      <AuthContext.Provider value={loggedIn}>
        <IonApp>
            <IonReactRouter>
              <IonTabs>
                <IonRouterOutlet>
                    <Route exact path="/home">
                      <Home />
                    </Route>
                    <Route exact path="/organizations">
                      <Organizations />
                    </Route>
                    <Route exact path="/vault">
                      <Vault />
                    </Route>
                    <Route path="/chats">
                      <Chats />
                    </Route>
                    <Route exact path="/aboutUs">
                      <AboutUs />
                    </Route>
                    <Route exact path="/">
                      <Redirect to='/home'></Redirect>
                    </Route>
                </IonRouterOutlet>
                {loggedIn ? (
                  <IonTabBar slot="bottom">
                    <IonTabButton tab="home" href="/home" >
                      <IonIcon aria-hidden="true" icon={home} />
                      <IonLabel>Homepage</IonLabel>
                    </IonTabButton>
                    <IonTabButton tab="organizations" href="/organizations">
                      <IonIcon aria-hidden="true" icon={people} />
                      <IonLabel>Organizations</IonLabel>
                    </IonTabButton>
                    <IonTabButton tab="vault" href="/vault">
                      <IonIcon aria-hidden="true" icon={lockClosed} />
                      <IonLabel>Vault</IonLabel>
                    </IonTabButton>
                    <IonTabButton tab="chats" href="/chats">
                      <IonIcon aria-hidden="true" icon={mail} />
                      <IonLabel>Chats</IonLabel>
                    </IonTabButton>
                  </IonTabBar>
                ) : (
                  <IonTabBar slot="bottom">
                    <IonTabButton tab="home" href="/home">
                      <IonIcon aria-hidden="true" icon={home} />
                      <IonLabel>Homepage</IonLabel>
                    </IonTabButton>
                    <IonTabButton tab="aboutUs" href="/aboutUs">
                      <IonIcon aria-hidden="true" icon={help} />
                      <IonLabel>AboutUs</IonLabel>
                    </IonTabButton>
                  </IonTabBar>
                )}
              </IonTabs>
            </IonReactRouter>
        </IonApp>
      </AuthContext.Provider>
    </UserContext.Provider>
  );
};

export default App;
