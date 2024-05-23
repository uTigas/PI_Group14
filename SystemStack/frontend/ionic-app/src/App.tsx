import { Redirect, Route } from 'react-router-dom';
import {
  IonApp,
  IonButtons,
  IonAvatar,
  IonButton,
  IonCol,
  IonGrid,
  IonHeader,
  IonIcon,
  IonItem,
  IonLabel,
  IonList,
  IonPopover,
  IonRouterOutlet,
  IonRow,
  IonTabBar,
  IonTabButton,
  IonTabs,
  IonText,
  IonTitle,
  IonToolbar,
  setupIonicReact
} from '@ionic/react';
import { IonReactRouter } from '@ionic/react-router';
import { cogOutline, contrastOutline, ellipse, exit, help, home, lockClosed, mail, people, person, personCircle, personCircleOutline, square, triangle } from 'ionicons/icons';
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
import { useContext, useEffect, useState } from 'react';
import AboutUs from './pages/AboutUs';
import Vault from './pages/Vault';
import React from 'react';
import ApiWrapper from './support/APIWrapper';
import Organization from './pages/Organization';
import OrganizationVault from './pages/OrganizationVault';
import "./support/General.css"

export interface User {
  username: string;
  first_name: string;
  last_name: string;
  email: string;
  phone: string;
}

export const AuthContext = React.createContext(false);
export const UserContext = React.createContext<User | null>(null);

setupIonicReact();

const App: React.FC = () => {
  const [loggedIn, setLoggedIn] = useState(false);
  const [userDetails, setUserDetails] = useState<User | null>(null);
  
  const checkAuthentication = async () => {
    const response = await ApiWrapper.checkAuthentication()
    if (response){
      setLoggedIn(response.data.is_authenticated);
      fetchUserDetails();
    } 
  };

  const fetchUserDetails = async () => {
    const response = await ApiWrapper.fetchUserDetails()
    if (response){
      setUserDetails(response.data.user);
    }
  }

  useEffect(() => {
    checkAuthentication();
  }, []); 

  return (
    <UserContext.Provider value={userDetails}>
      <AuthContext.Provider value={loggedIn}>
          <IonApp>
            <IonHeader>
              <IonToolbar>
                <IonGrid>
                  <IonRow className="ion-align-items-center">
                    <IonCol size='auto' className="ion-text-center">
                      <IonTitle>Vault</IonTitle>
                    </IonCol>
                    {loggedIn && userDetails ? (
                      <>
                      <IonCol className='ion-text-end'>
                        <IonButton id='view-profile' fill="outline" shape='round'>
                          <IonIcon slot="end" icon={personCircle}></IonIcon>
                          {userDetails.username}
                        </IonButton>
                        <IonPopover trigger='view-profile' triggerAction='click'>
                          <IonList>
                            <IonItem>
                              <IonIcon size='large' icon={personCircle}></IonIcon>
                            </IonItem>
                            
                            <IonItem>
                              <h6>Welcome, <strong style={{'color':'blue'}}>{userDetails.first_name} {userDetails.last_name}!</strong></h6>
                            </IonItem>

                            <IonItem>
                              <IonButton size='default' color="primary">
                                <IonIcon icon={contrastOutline}/>
                              </IonButton>
                              <IonLabel className='ion-padding-start'>Change Theme</IonLabel>
                            </IonItem>

                            <IonItem>
                              <IonButton size='default' color="tertiary">
                                <IonIcon icon={cogOutline}/>
                              </IonButton>
                              <IonLabel className='ion-padding-start'>Change Settings</IonLabel>
                            </IonItem>

                            <IonItem>
                              <IonButton href={ApiWrapper.backendURI + "logout"} size='default' color="danger">
                                <IonIcon icon={exit}/>
                              </IonButton>
                              <IonLabel className='ion-padding-start'>Logout</IonLabel>
                            </IonItem>
                          </IonList>
                        </IonPopover>
                      </IonCol>
                      </>
                    ) : (
                      <IonCol className="ion-text-end">
                        <IonButton href={ApiWrapper.backendURI + "login"} color="success">Login</IonButton>
                      </IonCol>
                    )}
                  </IonRow>
                </IonGrid>
              </IonToolbar>
            </IonHeader>
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
                    <Vault/>
                  </Route>
                  <Route path="/chats">
                    <Chats />
                  </Route>
                  <Route exact path="/aboutUs">
                    <AboutUs />
                  </Route>
                  <Route exact path="/organization/:id">
                      <Organization/>
                  </Route>
                  <Route exact path="/organization/vault/:id">
                      <OrganizationVault />
                  </Route>
                  <Route exact path="/">
                    <Redirect to='/home'></Redirect>
                  </Route>
                </IonRouterOutlet>
                {loggedIn ?(
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
                ):(
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
