import { IonButton, IonCol, IonContent, IonFooter, IonGrid, IonHeader, IonInput, IonItem, IonList, IonMenu, IonPage, IonRow, IonSplitPane, IonTitle, IonToolbar, IonAvatar, IonLabel } from '@ionic/react';
import ExploreContainer from '../components/ExploreContainer';
import AppAppBar from '../components/AppAppBar';

import './Chats.css';


import { useRef, useState } from 'react';


const Chats: React.FC = () => {
  const [activeChat, setActiveChat] = useState<string>('chat1');

  return (
    <IonPage>
      <IonHeader>
        <AppAppBar title='Chats' />
      </IonHeader>
      <IonContent>
        
      </IonContent>
    </IonPage>
  );
};


interface ChatRoomProps {
  activeChat: string;
  setActiveChat: (chatId: string) => void;
}

function ChatRoom({ activeChat, setActiveChat }: ChatRoomProps) {
  const dummy = useRef<HTMLSpanElement>(null);
  

  const contacts = [
    { id: 'chat1', name: 'Alice Johnson', avatar: 'https://i.pravatar.cc/150?img=1' },
    { id: 'chat2', name: 'Bob Smith', avatar: 'https://i.pravatar.cc/150?img=2' },
    { id: 'chat3', name: 'Charlie Brown', avatar: 'https://i.pravatar.cc/150?img=3' },
    { id: 'chat4', name: 'Diana Prince', avatar: 'https://i.pravatar.cc/150?img=4' },
    { id: 'chat5', name: 'Ethan Hunt', avatar: 'https://i.pravatar.cc/150?img=5' },
    { id: 'chat6', name: 'Fiona Gallagher', avatar: 'https://i.pravatar.cc/150?img=6' },
    { id: 'chat7', name: 'George Clooney', avatar: 'https://i.pravatar.cc/150?img=7' },
    { id: 'chat8', name: 'Hannah Montana', avatar: 'https://i.pravatar.cc/150?img=8' },
    { id: 'chat9', name: 'Ian Somerhalder', avatar: 'https://i.pravatar.cc/150?img=9' },
    { id: 'chat10', name: 'Jessica Alba', avatar: 'https://i.pravatar.cc/150?img=10' }
  ];

  return (
    <IonSplitPane when='xs' contentId='main'>
      <IonMenu contentId='main'>
        <IonHeader>
          <IonToolbar color='tertiary'>
            <IonTitle>Contacts</IonTitle>
          </IonToolbar>
        </IonHeader>
        <IonContent>
          <IonList>
            {contacts.map(contact => (
              <IonItem key={contact.id} button>
                <IonAvatar slot="start">
                  <img src={contact.avatar} alt={`Avatar of ${contact.name}`} />
                </IonAvatar>
                <IonLabel>{contact.name}</IonLabel>
              </IonItem>
            ))}
          </IonList>
        </IonContent>
      </IonMenu>
      <div className='ion-page' id='main'>
        <IonHeader>
          <IonToolbar>
            <IonTitle>
            </IonTitle>
          </IonToolbar>
        </IonHeader>
        <IonContent className='ion-padding'>
          <div className='Chat'>
            
          </div>
        </IonContent>
        <IonFooter>
          <IonToolbar>
            <IonGrid>
              <IonRow>
                <IonCol>
                  <IonInput aria-label="text" placeholder='Enter text'></IonInput>
                </IonCol>
                <IonCol size='auto' className='ion-text-end'>
                  <IonButton fill='outline'>Send</IonButton>
                </IonCol>
              </IonRow>
            </IonGrid>
          </IonToolbar>
        </IonFooter>
      </div>
    </IonSplitPane>
  )
}

function ChatMessage(props: { message: { text: any; uid: any; photoURL: any; }; }) {
  const { text, uid, photoURL } = props.message;
  const messageClass = uid;

  return (
    <>
      <div className={`message ${messageClass}`}>
        <img src={"https://ionicframework.com/docs/img/demos/avatar.svg"} alt="Avatar" />
        <p>{text}</p>
      </div>
    </>
  )
}

export default Chats;
