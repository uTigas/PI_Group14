import React, { useState, useContext, useEffect } from 'react';
import { AuthContext, URIContext, UserContext } from '../App';
import { IonButton, IonCol, IonGrid, IonIcon, IonItem, IonLabel, IonList, IonPopover, IonRow, IonTitle, IonToolbar } from '@ionic/react';
import { cogOutline, contrastOutline, exit, personCircle } from 'ionicons/icons';

const AppAppBar: React.FC<{ title: string }> = ({ title }) => {
    const logged = useContext(AuthContext);
    const userDetails = useContext(UserContext);
    const [getTitle, setTitle] = useState("");

    return (
        <IonToolbar>
            <IonGrid>
                <IonRow className="ion-align-items-center">
                    <IonCol size='auto' className="ion-text-center">
                        <IonTitle>{title}</IonTitle>
                    </IonCol>
                    { logged && userDetails ? (
                        <>
                            <IonCol className='ion-text-end'>
                                <IonButton id='view-profile' fill="outline">
                                    <IonIcon slot="end" icon={personCircle}></IonIcon>
                                    {userDetails.username}
                                </IonButton>
                                <IonPopover trigger='view-profile' triggerAction='click'>
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
                                            <IonButton size='default' color="danger">
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
                            <IonButton href={useContext(URIContext) + "login"} color="success">Login</IonButton>
                        </IonCol>
                    )}
                </IonRow>
            </IonGrid>
        </IonToolbar>
    );
};

export default AppAppBar;