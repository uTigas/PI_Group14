import React, { useState, useContext, useId } from 'react';
import { AuthContext, UserContext } from '../App';
import { IonButton, IonCol, IonGrid, IonIcon, IonItem, IonLabel, IonList, IonPopover, IonRow, IonTitle, IonToolbar } from '@ionic/react';
import { cogOutline, contrastOutline, exit, personCircle } from 'ionicons/icons';
import ApiWrapper from '../support/APIWrapper';

const AppAppBar: React.FC<{ title: string }> = ({ title }) => {
    const logged = useContext(AuthContext);
    const userDetails = useContext(UserContext);
    const [getTitle, setTitle] = useState("");

    // @Note: for shared headers in tabs if we don't generate a unique id for the popup button 
    // there's a bug where the popup doesn't open on other tabs other than the one firstly rendered (Ctrl+R).
    const id = useId();

    return (
        <IonToolbar>
            <IonGrid>
                <IonRow className="ion-align-items-center">
                    <IonCol size='auto' className="ion-text-center">
                        <IonTitle>{title}</IonTitle>
                    </IonCol>
                    {logged && userDetails ? (
                        <>
                            <IonCol className='ion-text-end'>
                                <IonButton id={id} fill="outline">
                                    <IonIcon slot="end" icon={personCircle}></IonIcon>
                                    {userDetails.username}
                                </IonButton>
                                <IonPopover trigger={id} triggerAction='click'>
                                    <IonList>
                                        <IonItem>
                                            <h6>Welcome, {userDetails.first_name} {userDetails.last_name}!</h6>
                                        </IonItem>

                                        <IonItem>
                                            <IonButton size='default' color="primary">
                                                <IonIcon icon={contrastOutline} />
                                            </IonButton>
                                            <IonLabel className='ion-padding-start'>Change Theme</IonLabel>
                                        </IonItem>

                                        <IonItem>
                                            <IonButton size='default' color="tertiary">
                                                <IonIcon icon={cogOutline} />
                                            </IonButton>
                                            <IonLabel className='ion-padding-start'>Change Settings</IonLabel>
                                        </IonItem>

                                        <IonItem>
                                            <IonButton size='default' color="danger" href={ApiWrapper.backendURI + "logout"}>
                                                <IonIcon icon={exit} />
                                            </IonButton>
                                            <IonLabel className='ion-padding-start'>Logout</IonLabel>
                                        </IonItem>
                                    </IonList>
                                </IonPopover>
                            </IonCol>
                        </>
                    ) : (
                        <IonCol className="ion-text-end">
                            <IonButton href='/register' color="primary">Register</IonButton>
                        </IonCol>
                    )}
                </IonRow>
            </IonGrid>
        </IonToolbar>
    );
};

export default AppAppBar;